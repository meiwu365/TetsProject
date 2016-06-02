// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
aaaaaaaaaaaaaaaaaaaa
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/Composites/BTComposite_SimpleParallel.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BehaviorTree/Decorators/BTDecorator_Loop.h"
#include "BehaviorTree/Decorators/BTDecorator_ForceSuccess.h"
#include "BehaviorTree/Tasks/BTTask_RunBehavior.h"

#include "BehaviorTree/TestBTTask_Log.h"
#include "BehaviorTree/TestBTTask_SetFlag.h"
#include "BehaviorTree/TestBTTask_LatentWithFlags.h"
#include "BehaviorTree/TestBTDecorator_DelayedAbort.h"

struct FBTBuilder
{
	static UBehaviorTree& CreateBehaviorTree()
	{
		UBlackboardData* BB = NewObject<UBlackboardData>();
		FBlackboardEntry KeyData;

		KeyData.EntryName = TEXT("Bool1");
		KeyData.KeyType = NewObject<UBlackboardKeyType_Bool>();
		BB->Keys.Add(KeyData);

		KeyData.EntryName = TEXT("Bool2");
		KeyData.KeyType = NewObject<UBlackboardKeyType_Bool>();
		BB->Keys.Add(KeyData);

		KeyData.EntryName = TEXT("Bool3");
		KeyData.KeyType = NewObject<UBlackboardKeyType_Bool>();
		BB->Keys.Add(KeyData);

		KeyData.EntryName = TEXT("Bool4");
		KeyData.KeyType = NewObject<UBlackboardKeyType_Bool>();
		BB->Keys.Add(KeyData);

		BB->UpdateParentKeys();

		UBehaviorTree* TreeOb = NewObject<UBehaviorTree>();
		TreeOb->BlackboardAsset = BB;

		return *TreeOb;
	}

	static UBehaviorTree& CreateBehaviorTree(UBehaviorTree& ParentTree)
	{
		UBehaviorTree* TreeOb = NewObject<UBehaviorTree>();
		TreeOb->BlackboardAsset = ParentTree.BlackboardAsset;
		
		return *TreeOb;
	}

	static UBTComposite_Selector& AddSelector(UBehaviorTree& TreeOb)
	{
		UBTComposite_Selector* NodeOb = NewObject<UBTComposite_Selector>(&TreeOb);
		NodeOb->InitializeFromAsset(TreeOb);
		TreeOb.RootNode = NodeOb;
		return *NodeOb;
	}

	static UBTComposite_Selector& AddSelector(UBTCompositeNode& ParentNode)
	{
		UBTComposite_Selector* NodeOb = NewObject<UBTComposite_Selector>(ParentNode.GetTreeAsset());
		NodeOb->InitializeFromAsset(*ParentNode.GetTreeAsset());

		const int32 ChildIdx = ParentNode.Children.AddZeroed(1);
		ParentNode.Children[ChildIdx].ChildComposite = NodeOb;

		return *NodeOb;
	}

	static UBTComposite_Sequence& AddSequence(UBehaviorTree& TreeOb)
	{
		UBTComposite_Sequence* NodeOb = NewObject<UBTComposite_Sequence>(&TreeOb);
		NodeOb->InitializeFromAsset(TreeOb);
		TreeOb.RootNode = NodeOb;
		return *NodeOb;
	}

	static UBTComposite_Sequence& AddSequence(UBTCompositeNode& ParentNode)
	{
		UBTComposite_Sequence* NodeOb = NewObject<UBTComposite_Sequence>(ParentNode.GetTreeAsset());
		NodeOb->InitializeFromAsset(*ParentNode.GetTreeAsset());

		const int32 ChildIdx = ParentNode.Children.AddZeroed(1);
		ParentNode.Children[ChildIdx].ChildComposite = NodeOb;

		return *NodeOb;
	}

	static UBTComposite_SimpleParallel& AddParallel(UBehaviorTree& TreeOb, EBTParallelMode::Type Mode)
	{
		UBTComposite_SimpleParallel* NodeOb = NewObject<UBTComposite_SimpleParallel>(&TreeOb);
		NodeOb->FinishMode = Mode;
		NodeOb->InitializeFromAsset(TreeOb);
		TreeOb.RootNode = NodeOb;
		return *NodeOb;
	}

	static UBTComposite_SimpleParallel& AddParallel(UBTCompositeNode& ParentNode, EBTParallelMode::Type Mode)
	{
		UBTComposite_SimpleParallel* NodeOb = NewObject<UBTComposite_SimpleParallel>(ParentNode.GetTreeAsset());
		NodeOb->FinishMode = Mode;
		NodeOb->InitializeFromAsset(*ParentNode.GetTreeAsset());

		const int32 ChildIdx = ParentNode.Children.AddZeroed(1);
		ParentNode.Children[ChildIdx].ChildComposite = NodeOb;

		return *NodeOb;
	}

	static void AddTask(UBTCompositeNode& ParentNode, int32 LogIndex, EBTNodeResult::Type NodeResult, int32 ExecutionTicks = 0)
	{
		UTestBTTask_Log* TaskNode = NewObject<UTestBTTask_Log>(ParentNode.GetTreeAsset());
		TaskNode->LogIndex = LogIndex;
		TaskNode->LogResult = NodeResult;
		TaskNode->ExecutionTicks = ExecutionTicks;

		const int32 ChildIdx = ParentNode.Children.AddZeroed(1);
		ParentNode.Children[ChildIdx].ChildTask = TaskNode;
	}

	static void AddTaskLogFinish(UBTCompositeNode& ParentNode, int32 LogIndex, int32 FinishIndex, EBTNodeResult::Type NodeResult, int32 ExecutionTicks = 0)
	{
		UTestBTTask_Log* TaskNode = NewObject<UTestBTTask_Log>(ParentNode.GetTreeAsset());
		TaskNode->LogIndex = LogIndex;
		TaskNode->LogFinished = FinishIndex;
		TaskNode->LogResult = NodeResult;
		TaskNode->ExecutionTicks = ExecutionTicks;

		const int32 ChildIdx = ParentNode.Children.AddZeroed(1);
		ParentNode.Children[ChildIdx].ChildTask = TaskNode;
	}

	static void AddTaskFlagChange(UBTCompositeNode& ParentNode, bool bValue, EBTNodeResult::Type NodeResult, FName BoolKeyName = TEXT("Bool1"))
	{
		UTestBTTask_SetFlag* TaskNode = NewObject<UTestBTTask_SetFlag>(ParentNode.GetTreeAsset());
		TaskNode->bValue = bValue;
		TaskNode->TaskResult = NodeResult;
		TaskNode->KeyName = BoolKeyName;

		const int32 ChildIdx = ParentNode.Children.AddZeroed(1);
		ParentNode.Children[ChildIdx].ChildTask = TaskNode;
	}

	static void AddTaskSubtree(UBTCompositeNode& ParentNode, UBehaviorTree* TreeAsset)
	{
		UBTTask_RunBehavior* TaskNode = NewObject<UBTTask_RunBehavior>(ParentNode.GetTreeAsset());

		UObjectProperty* SubtreeProp = FindField<UObjectProperty>(UBTTask_RunBehavior::StaticClass(), TEXT("BehaviorAsset"));
		uint8* SubtreePropData = SubtreeProp->ContainerPtrToValuePtr<uint8>(TaskNode);
		SubtreeProp->SetObjectPropertyValue(SubtreePropData, TreeAsset);

		const int32 ChildIdx = ParentNode.Children.AddZeroed(1);
		ParentNode.Children[ChildIdx].ChildTask = TaskNode;
	}

	static void AddTaskLatentFlags(UBTCompositeNode& ParentNode, EBTNodeResult::Type NodeResult,
		int32 ExecuteHalfTicks, FName ExecuteKeyName, int32 ExecuteLogStart, int32 ExecuteLogFinish,
		int32 AbortHalfTicks = 0, FName AbortKeyName = NAME_None, int32 AbortLogStart = 0, int32 AbortLogFinish = 0)
	{
		UTestBTTask_LatentWithFlags* TaskNode = NewObject<UTestBTTask_LatentWithFlags>(ParentNode.GetTreeAsset());
		TaskNode->ExecuteTicks = ExecuteHalfTicks;
		TaskNode->KeyNameExecute = ExecuteKeyName;
		TaskNode->LogIndexExecuteStart = ExecuteLogStart;
		TaskNode->LogIndexExecuteFinish = ExecuteLogFinish;
		TaskNode->AbortTicks = AbortHalfTicks;
		TaskNode->KeyNameAbort = AbortKeyName;
		TaskNode->LogIndexAbortStart = AbortLogStart;
		TaskNode->LogIndexAbortFinish = AbortLogFinish;

		const int32 ChildIdx = ParentNode.Children.AddZeroed(1);
		ParentNode.Children[ChildIdx].ChildTask = TaskNode;
	}

	template<class T>
	static T& WithDecorator(UBTCompositeNode& ParentNode, UClass* DecoratorClass = T::StaticClass())
	{
		T* DecoratorOb = NewObject<T>(ParentNode.GetTreeAsset());
		ParentNode.Children.Last().Decorators.Add(DecoratorOb);

		return *DecoratorOb;
	}

	static void WithDecoratorBlackboard(UBTCompositeNode& ParentNode, EBasicKeyOperation::Type Condition, EBTFlowAbortMode::Type Observer, FName BoolKeyName = TEXT("Bool1"))
	{
		UBTDecorator_Blackboard& BBDecorator = WithDecorator<UBTDecorator_Blackboard>(ParentNode);
		
		UByteProperty* ConditionProp = FindField<UByteProperty>(UBTDecorator_Blackboard::StaticClass(), TEXT("OperationType"));
		uint8* ConditionPropData = ConditionProp->ContainerPtrToValuePtr<uint8>(&BBDecorator);
		ConditionProp->SetIntPropertyValue(ConditionPropData, (uint64)Condition);

		UByteProperty* ObserverProp = FindField<UByteProperty>(UBTDecorator_Blackboard::StaticClass(), TEXT("FlowAbortMode"));
		uint8* ObserverPropData = ObserverProp->ContainerPtrToValuePtr<uint8>(&BBDecorator);
		ObserverProp->SetIntPropertyValue(ObserverPropData, (uint64)Observer);

		UStructProperty* KeyProp = FindField<UStructProperty>(UBTDecorator_Blackboard::StaticClass(), TEXT("BlackboardKey"));
		FBlackboardKeySelector* KeyPropData = KeyProp->ContainerPtrToValuePtr<FBlackboardKeySelector>(&BBDecorator);
		KeyPropData->SelectedKeyName = BoolKeyName;
	}

	static void WithDecoratorDelayedAbort(UBTCompositeNode& ParentNode, int32 NumTicks, bool bAbortOnlyOnce = true)
	{
		UTestBTDecorator_DelayedAbort& AbortDecorator = WithDecorator<UTestBTDecorator_DelayedAbort>(ParentNode);
		AbortDecorator.DelayTicks = NumTicks;
		AbortDecorator.bOnlyOnce = bAbortOnlyOnce;
	}

	static void WithDecoratorLoop(UBTCompositeNode& ParentNode, int32 NumLoops = 2)
	{
		UBTDecorator_Loop& LoopDecorator = WithDecorator<UBTDecorator_Loop>(ParentNode);
		LoopDecorator.NumLoops = NumLoops;
	}
};
