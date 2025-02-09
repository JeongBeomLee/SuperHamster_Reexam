#include "pch.h"
#include "Component.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "TransformAnimator.h"

Component::Component(COMPONENT_TYPE type) : Object(OBJECT_TYPE::COMPONENT), _type(type)
{

}

Component::~Component()
{
}

shared_ptr<GameObject> Component::GetGameObject()
{
	return _gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
	return _gameObject.lock()->GetTransform();
}

shared_ptr<MeshRenderer> Component::GetMeshRenderer()
{
	return _gameObject.lock()->GetMeshRenderer();
}

shared_ptr<Animator> Component::GetAnimator()
{
	return _gameObject.lock()->GetAnimator();
}

shared_ptr<TransformAnimator> Component::GetTransformAnimator()
{
	return _gameObject.lock()->GetTransformAnimator();
}
