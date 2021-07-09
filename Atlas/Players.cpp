#include "RBXClasses.h"
#include "Memory.h"

Player Players::GetLocalPlayer() {
	return static_cast<Player>(Memory::GetPointerAddress(this->Address + 0x128));
}

std::vector<Player> Players::GetPlayers() {
	std::vector<Player> players = {};

	std::vector<Instance> children = this->GetChildren();

	for (Instance child : children) {
		if (child.GetClassType() == "Player") {
			players.push_back(static_cast<Player>(child.Address));
		}
	}

	return players;
}