#pragma once



class SupportModule
{
public:
	SupportModule();
	~SupportModule();

	BOOL ShowHelpDialog();
	BOOL ShowSettingsDialog();

private:
	class SupportModuleImpl* pImpl;
};

