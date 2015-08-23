#include "Block.h"

std::map<std::string, Block*> Block::blocks = std::map<std::string, Block*>();
Block* Block::blocksByIds [256];

void Block::registerBlocks() {
    new Block(1, "stone");
}

Block::Block(int id, std::string stringId) : Item(id, stringId) {
    blocks[stringId] = this;
    blocksByIds[id] = this;
}