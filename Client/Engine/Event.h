#pragma once
#include "pch.h"
#include "Logger.h"

namespace Event
{
    // 이벤트 인터페이스
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
    
    // 이벤트 콜백
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

    // 이벤트 큐
    template<typename EventType>
    class EventQueue {
    public:
        void Push(const EventType& event) {
            m_events.push(event);
            Logger::Instance().Debug("이벤트가 큐에 추가됨: {}", typeid(EventType).name());
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

	// 이벤트 디스패처
    template<typename EventType>
    class EventDispatcher {
    public:
        using HandlerId = size_t;

        HandlerId Subscribe(EventCallback<EventType> callback) {
            HandlerId id = m_nextHandlerId++;
            m_handlers.insert({ id, std::move(callback) });
            Logger::Instance().Debug("이벤트 핸들러가 등록됨. ID: {}, 이벤트 타입: {}",
                id, typeid(EventType).name());
            return id;
        }

        void Unsubscribe(HandlerId id) {
            size_t erased = m_handlers.erase(id);
            if (erased > 0) {
                Logger::Instance().Debug("이벤트 핸들러가 제거됨. ID: {}", id);
            }
        }

        void Dispatch(const EventType& event) {
            // 만약 핸들러 등록 도중 수정이 일어날 가능성이 있다면 복사본을 만들어서 순회합니다.
            auto handlers = m_handlers; // snapshot
            for (auto& [id, handler] : handlers) {
                try {
                    handler(event);
                }
                catch (const std::exception& e) {
                    Logger::Instance().Error("이벤트 핸들러 실행 실패. ID: {}, 오류: {}",
                        id, e.what());
                }
            }
        }

    private:
        std::unordered_map<HandlerId, EventCallback<EventType>> m_handlers;
        HandlerId m_nextHandlerId = 1;
    };
}