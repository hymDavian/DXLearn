#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();
	//初始化所有按键都是抬起的
	void Initialize();
	//标记一个按键按下
	void KeyDown(unsigned int);
	//标记一个按键抬起
	void KeyUp(unsigned int);
	//判断某个按键是否抬起
	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];
};

#endif
