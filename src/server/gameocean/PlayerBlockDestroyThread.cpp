#include "PlayerBlockDestroyThread.h"

#include <chrono>
#include "Player.h"

void PlayerBlockDestroyThread::run() {
    int waitTime = -1;
    while(true) {
        {
            std::unique_lock<std::mutex> lock(notifyMutex);
            if (waitTime > 0) {
                notify.wait_for(lock, std::chrono::milliseconds(waitTime));
            } else {
                notify.wait(lock);
            }
            if (shouldStop) return;
        }

        waitTime = -1;
        std::vector<Player*> players = server.getPlayers();
        for (Player* player : players) {
            if (player->miningBlock != nullptr) {
                int r = player->getRemainingMiningTime();
                if (r <= 0) {
                    // mined
                    player->finishedMining();
                } else if (waitTime == -1 || waitTime > r) {
                    waitTime = r;
                }
            }
        }
    }
}