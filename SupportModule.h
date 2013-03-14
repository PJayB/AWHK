#pragma once



class SupportModule
{
public:
	SupportModule();
	~SupportModule();

	BOOL ShowSettingsDialog();

private:
	class SupportModuleImpl* pImpl;
};

