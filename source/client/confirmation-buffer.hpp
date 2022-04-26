#pragma once
#include <array>
#include <functional>
#include <inttypes.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

class BaseMessage
{
private:
    // Serves as unique identifier
    uint64_t sequenceNumber;

public:
    BaseMessage(uint64_t sequenceNumber) : sequenceNumber(sequenceNumber) {}

    uint64_t GetSequenceNumber() const { return sequenceNumber; }
};

struct ConfirmableItem
{
    BaseMessage* item      = nullptr;
    bool         confirmed = false;

    void Confirm() { confirmed = true; }
};

template<int N>
struct ConfirmableContainer
{
    std::array<ConfirmableItem, N> content;
    uint64_t                       originalSeqn = 0;
};

template<int N>
class ConfirmationBuffer
{

public:
    using ItemType   = ConfirmableContainer<N>;
    using CallbackFn = void(ItemType&);

    ConfirmationBuffer(std::function<CallbackFn> callback) : callback(callback) {}

    void Push(std::array<BaseMessage*, N>& items)
    {
        ItemType container;

        for (int i = 0; i < N; i++)
        {
            container.content[i].confirmed = false;
            container.content[i].item      = items[i];
        }

        buffer.emplace_back(std::move(container));
    }

    void Push(std::array<BaseMessage*, N>& items, uint64_t originalSeqn)
    {
        ItemType container;

        for (int i = 0; i < N; i++)
        {
            container.content[i].confirmed = false;
            container.content[i].item      = items[i];
        }

        container.originalSeqn = originalSeqn;

        buffer.emplace_back(std::move(container));
    }
    // T& Get(std::size_t containerIdx, std::size_t itemIdx)
    // {
    //     return buffer[containerIdx][itemIdx].item.GetContent();
    // }

    int Confirm(uint64_t sequenceNumber)
    {
        int confirmed = 0;
        for (auto& container : buffer)
        {
            for (auto& item : container.content)
            {
                auto seqn = item.item->GetSequenceNumber();
                if (seqn == sequenceNumber)
                {
                    item.Confirm();
                    confirmed++;
                    std::cout << "Confirmed seqn " << seqn << std::endl;
                }
            }

            if (ContainerConfirmed(container))
            {
                if (callback)
                {
                    std::cout << "Callback" << std::endl;
                    callback(container);
                }
            }
        }

        return confirmed;
    }

private:
    std::vector<ItemType> buffer;
    std::mutex            bufferMutex;

    /** Function that is called whenever a confirmable container is confirmed */
    std::function<CallbackFn> callback;

    bool ContainerConfirmed(ItemType& container)
    {
        bool confirmed = true;
        for (int i = 0; i < N; i++)
        {
            if (container.content[i].confirmed == false) { confirmed = false; }
        }

        return confirmed;
    }
};
