#pragma once
enum class TableItemType {
	Error, // δƥ�����Ŀ
	S, // �ƽ�
	R, // ��Լ
	Goto, // GOTO ��Ŀ
	Accept // ����
};
enum class LRType
{
	LR0,
	SLR1
};
// �����ͻ������
enum class CoflictType
{
	Normal, // �޳�ͻ
	SR,  // ��Լ-�ƽ���ͻ
	RR   // ��Լ-��Լ��ͻ
};
enum class PInJizuType
{
	Accept,  // ������
	ToReduce,  // ��Լ��
	ToShift  // �ƽ����ߴ�Լ�� A->a.Xb ֮���
};