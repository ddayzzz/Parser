#pragma once
enum class TableItemType {
	Error, // 未匹配的项目
	S, // 移进
	R, // 规约
	Goto, // GOTO 项目
	Accept // 接受
};
enum class LRType
{
	LR0,
	SLR1
};
// 定义冲突的类型
enum class CoflictType
{
	Normal, // 无冲突
	SR,  // 规约-移进冲突
	RR   // 规约-规约冲突
};
enum class PInJizuType
{
	Accept,  // 接受项
	ToReduce,  // 规约项
	ToShift  // 移进或者待约项 A->a.Xb 之类的
};