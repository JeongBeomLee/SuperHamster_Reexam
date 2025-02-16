#pragma once
#include "EventTypes.h"

class EventManager {
public:
    static EventManager* GetInstance() {
        static EventManager instance;
        return &instance;
    }

    // 이벤트 발행
    template<typename EventType>
    void Publish(const EventType& event) {
        GetQueue<EventType>().Push(event);
        Logger::Instance().Debug("이벤트 발행: {}", typeid(EventType).name());
    }

    template<typename EventType>
    Event::EventDispatcher<EventType>::HandlerId Subscribe(
		Event::EventCallback<EventType> callback) {
        return GetDispatcher<EventType>().Subscribe(std::move(callback));
    }

    template<typename EventType>
    void Unsubscribe(typename Event::EventDispatcher<EventType>::HandlerId id) {
        GetDispatcher<EventType>().Unsubscribe(id);
    }

    // 이벤트 처리 (메인 스레드에서만 호출)
    void Update() {
        ProcessEvents<Event::CollisionEvent>();
        ProcessEvents<Event::InputEvent>();

		ProcessEvents<Event::ProjectileHitEvent>();
		ProcessEvents<Event::PlayerHitEvent>();
		ProcessEvents<Event::TriggerEvent>();

		ProcessEvents<Event::SceneChangeEvent>();
    }

private:
    EventManager() = default;
    ~EventManager() = default;

    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    template<typename EventType>
    Event::EventQueue<EventType>& GetQueue() {
        return std::get<Event::EventQueue<EventType>>(m_queues);
    }

    template<typename EventType>
    Event::EventDispatcher<EventType>& GetDispatcher() {
        return std::get<Event::EventDispatcher<EventType>>(m_dispatchers);
    }

    template<typename EventType>
    void ProcessEvents() {
        auto& queue = GetQueue<EventType>();
        auto& dispatcher = GetDispatcher<EventType>();

        EventType event;
        while (queue.Pop(event)) {
            dispatcher.Dispatch(event);
        }
    }

    // 모든 이벤트 타입에 대한 큐와 디스패처를 저장
    std::tuple<
        Event::EventQueue<Event::CollisionEvent>,
        Event::EventQueue<Event::ProjectileHitEvent>,
        Event::EventQueue<Event::InputEvent>,
        Event::EventQueue<Event::PlayerHitEvent>,
        Event::EventQueue<Event::TriggerEvent>,
		Event::EventQueue<Event::SceneChangeEvent>
    > m_queues;

    std::tuple<
        Event::EventDispatcher<Event::CollisionEvent>,
        Event::EventDispatcher<Event::ProjectileHitEvent>,
        Event::EventDispatcher<Event::InputEvent>,
		Event::EventDispatcher<Event::PlayerHitEvent>,
        Event::EventDispatcher<Event::TriggerEvent>,
		Event::EventDispatcher<Event::SceneChangeEvent>
    > m_dispatchers;
};