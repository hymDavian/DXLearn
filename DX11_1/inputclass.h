#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();
	//��ʼ�����а�������̧���
	void Initialize();
	//���һ����������
	void KeyDown(unsigned int);
	//���һ������̧��
	void KeyUp(unsigned int);
	//�ж�ĳ�������Ƿ�̧��
	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];
};

#endif
