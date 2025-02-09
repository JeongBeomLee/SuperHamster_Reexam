#pragma once
#include "pch.h"
#include "Logger.h"

namespace Event
{
    // �̺�Ʈ �������̽�
    class IEvent {
    public:
        virtual ~IEvent() = default;

        const std::chrono::system_clock::time_point& GetTimestamp() const {
            return m_timestamp;
        }

    protected:
        IEvent() : m_timestamp(std::chrono::system_clock::now()) {}

    private:
        std::chrono::system_clock::time_point m_timestamp;
    };

    template<typename EventType>
    class IEventHandler {
    public:
        virtual ~IEventHandler() = default;
        virtual void Handle(const EventType& event) = 0;
    };
    
    // �̺�Ʈ �ݹ�
    template<typename EventType>
    class EventCallback {
    public:
        using CallbackFn = std::function<void(const EventType&)>;

        EventCallback(CallbackFn callback)
            : m_callback(std::move(callback)) {}

        void operator()(const EventType& event) {
            m_callback(event);
        }

    private:
        CallbackFn m_callback;
    };

    // �̺�Ʈ ť
    template<typename EventType>
    class EventQueue {
    public:
        void Push(const EventType& event) {
            m_events.push(event);
            Logger::Instance().Debug("�̺�Ʈ�� ť�� �߰���: {}", typeid(EventType).name());
        }

        bool Pop(EventType& event) {
            if (m_events.empty())
                return false;
            event = m_events.front();
            m_events.pop();
            return true;
        }

        bool IsEmpty() const {
            return m_events.empty();
        }

        size_t Size() const {
            return m_events.size();
        }

    private:
        std::queue<EventType> m_events;
    };

	// �̺�Ʈ ����ó
    template<typename EventType>
    class EventDispatcher {
    public:
        using HandlerId = size_t;

        HandlerId Subscribe(EventCallback<EventType> callback) {
            HandlerId id = m_nextHandlerId++;
            m_handlers.insert({ id, std::move(callback) });
            Logger::Instance().Debug("�̺�Ʈ �ڵ鷯�� ��ϵ�. ID: {}, �̺�Ʈ Ÿ��: {}",
                id, typeid(EventType).name());
            return id;
        }

        void Unsubscribe(HandlerId id) {
            size_t erased = m_handlers.erase(id);
            if (erased > 0) {
                Logger::Instance().Debug("�̺�Ʈ �ڵ鷯�� ���ŵ�. ID: {}", id);
            }
        }

        void Dispatch(const EventType& event) {
            // ���� �ڵ鷯 ��� ���� ������ �Ͼ ���ɼ��� �ִٸ� ���纻�� ���� ��ȸ�մϴ�.
            auto handlers = m_handlers; // snapshot
            for (auto& [id, handler] : handlers) {
                try {
                    handler(event);
                }
                catch (const std::exception& e) {
                    Logger::Instance().Error("�̺�Ʈ �ڵ鷯 ���� ����. ID: {}, ����: {}",
                        id, e.what());
                }
            }
        }

    private:
        std::unordered_map<HandlerId, EventCallback<EventType>> m_handlers;
        HandlerId m_nextHandlerId = 1;
    };
}