#include "PlayerChunkQueueThread.h"
#include "Server.h"
#include "Player.h"
#include <gameocean/utils/Logger.h>

#include <chrono>

void PlayerChunkQueueThread::run() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(server.sendChunksDelay));

        if (shouldStop)
            break;

        std::vector<std::shared_ptr<Player>> players = server.getPlayers();
        for (auto& player : players) {
            player->updateChunkQueue();
            player->sendQueuedChunks();
        }
    }
}