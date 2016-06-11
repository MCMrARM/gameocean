#pragma once

enum class RakNetReliability {
    UNRELIABLE, UNRELIABLE_SEQUENCED, RELIABLE, RELIABLE_ORDERED, RELIABLE_SEQUENCED,
    UNRELIABLE_ACK_RECEIPT, RELIABLE_ACK_RECEIPT, RELIABLE_ORDERED_ACK_RECEIPT
};

#define RakNetIsTypeReliable(type) (type == RakNetReliability::RELIABLE || type == RakNetReliability::RELIABLE_ORDERED || \
    type == RakNetReliability::RELIABLE_SEQUENCED || type == RakNetReliability::RELIABLE_ACK_RECEIPT || \
    type == RakNetReliability::RELIABLE_ORDERED_ACK_RECEIPT)
#define RakNetIsTypeUnreliable(type) (type == RakNetReliability::UNRELIABLE || type == RakNetReliability::UNRELIABLE_SEQUENCED || \
    type == RakNetReliability::UNRELIABLE_ACK_RECEIPT)
#define RakNetIsTypeSequenced(type) (type == RakNetReliability::UNRELIABLE_SEQUENCED || type == RakNetReliability::RELIABLE_SEQUENCED)
#define RakNetIsTypeOrdered(type) (type == RakNetReliability::RELIABLE_ORDERED || type == RakNetReliability::RELIABLE_ORDERED_ACK_RECEIPT || \
    type == RakNetReliability::UNRELIABLE_SEQUENCED || type == RakNetReliability::RELIABLE_SEQUENCED)
#define RakNetTypeNeedsACKReceipt(type) (type == RakNetReliability::UNRELIABLE_ACK_RECEIPT || \
    type == RakNetReliability::RELIABLE_ACK_RECEIPT || type == RakNetReliability::RELIABLE_ORDERED_ACK_RECEIPT)
