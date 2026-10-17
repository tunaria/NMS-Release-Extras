// nms_lon_session.cpp -- hands the client's DLL its Legends of Norrath login token.
//
// The card game (LegendsOfNorrath.dll inside eqgame) logs in to world's card service
// (world/lon_service.cpp) with a session string. That string is the ONLY proof of who the
// player is, so it must be something only this character's own client could know: a
// random token, written to lon_session for this account + character, and sent to the
// client our DLL already proved itself on (OP_NMSClientInfo). The DLL gives it to the card
// game as --sessionID= and relays the card game's 127.0.0.1:15782 to host:port below.
//
// A live token is re-sent on every zone-in rather than replaced, so a card game that is
// already logged in keeps working while the character zones.
//
// The same packet carries the card game's link-button switches (Custom:LoNLinkButtons) and their
// addresses (Custom:LoNHelpUrl / LoNForumsUrl / LoNLeaderboardUrl / LoNWebsiteUrl), so a rule change
// reaches the client on its next zone-in.

#include "zone/client.h"

#include "common/eq_packet_structs.h"
#include "common/rulesys.h"
#include "common/strings.h"
#include "zone/zonedb.h"
#include "zone/zone_config.h"

#include "fmt/format.h"

#include <cstring>
#include <random>
#include <string>

void Client::SendLoNSession()
{
	if (!RuleB(Custom, LoNEnabled)) {
		return;
	}

	std::string token;
	auto r = database.QueryDatabase(fmt::format(
		"SELECT token FROM lon_session WHERE account_id = {} AND character_id = {} "
		"AND expires_at > NOW() + INTERVAL 1 HOUR ORDER BY expires_at DESC LIMIT 1",
		AccountID(), CharacterID()));
	if (r.Success() && r.RowCount() > 0 && r.begin()[0]) {
		token = r.begin()[0];
	}
	else {
		std::random_device rd;
		static const char *hex = "0123456789abcdef";
		for (int i = 0; i < 4; ++i) {
			uint32 v = rd();
			for (int j = 0; j < 8; ++j) {
				token.push_back(hex[v & 15]);
				v >>= 4;
			}
		}
		database.QueryDatabase(fmt::format(
			"DELETE FROM lon_session WHERE account_id = {} AND expires_at < NOW()", AccountID()));
		auto w = database.QueryDatabase(fmt::format(
			"INSERT INTO lon_session (token, account_id, character_id, expires_at) "
			"VALUES ('{}', {}, {}, NOW() + INTERVAL 12 HOUR)",
			token, AccountID(), CharacterID()));
		if (!w.Success()) {
			LogError("[LoN] could not store a card game session for [{}]", GetCleanName());
			return;
		}
	}

	// Where the DLL relays the card game: the rule, else the address players already use
	// for this server's world, else this machine.
	std::string host = RuleS(Custom, LoNPublicHost);
	if (host.empty()) {
		host = ZoneConfig::get()->WorldAddress;
	}
	if (host.empty()) {
		host = "127.0.0.1";
	}

	// The card game's five navigator link items (Custom:LoNLinkButtons, bit set = ON) and the addresses
	// the four page buttons open. The DLL keeps a button grey unless its bit is on AND its address is an
	// http:// or https:// one; an over-long address is sent empty (= off), never cut short.
	const std::string urls[4] = {
		RuleS(Custom, LoNHelpUrl), RuleS(Custom, LoNForumsUrl), RuleS(Custom, LoNLeaderboardUrl), RuleS(Custom, LoNWebsiteUrl),
	};
	size_t tail = 4;
	for (const std::string &u : urls) {
		tail += 2 + (u.size() <= NMS_LONLINK_URL_MAX ? u.size() : 0);
	}

	auto outapp = new EQApplicationPacket(OP_NMSLoNSession, static_cast<uint32>(sizeof(NMSLoNSession_Struct) + tail));
	auto *s = reinterpret_cast<NMSLoNSession_Struct *>(outapp->pBuffer);
	strn0cpy(s->token, token.c_str(), sizeof(s->token));
	strn0cpy(s->host, host.c_str(), sizeof(s->host));
	s->port         = static_cast<uint32>(RuleI(Custom, LoNPort));
	s->link_buttons = static_cast<uint32>(RuleI(Custom, LoNLinkButtons)) & NMS_LONLINK_ALL;

	uint8 *p = outapp->pBuffer + sizeof(NMSLoNSession_Struct);
	const uint32 magic = NMS_LONLINK_MAGIC;
	memcpy(p, &magic, 4);
	p += 4;
	for (const std::string &u : urls) {
		const uint16 len = static_cast<uint16>(u.size() <= NMS_LONLINK_URL_MAX ? u.size() : 0);
		memcpy(p, &len, 2);
		p += 2;
		if (len) {
			memcpy(p, u.data(), len);
			p += len;
		}
	}
	QueuePacket(outapp);
	safe_delete(outapp);
}
