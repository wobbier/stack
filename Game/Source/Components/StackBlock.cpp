#include "StackBlock.h"
#include "imgui.h"

StackBlock::StackBlock()
	: Component("StackBlock")
	, Color(1.f)
{

}

void StackBlock::Init()
{

}

void StackBlock::OnSerialize(json& outJson)
{

}

void StackBlock::OnDeserialize(const json& inJson)
{

}

#if USING( ME_EDITOR )

void StackBlock::OnEditorInspect()
{
	ImGui::Checkbox("Movement Axis Z", &MoveOnX);
	ImGui::Checkbox("Block Direction", &BlockDirection);
	ImGui::DragFloat("Movement Speed", &BlockMoveSpeed);
}

#endif