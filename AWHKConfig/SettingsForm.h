#pragma once

namespace AWHKConfig {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for SettingsForm
	/// </summary>
	public ref class SettingsForm : public System::Windows::Forms::Form
	{
	public:
		SettingsForm()
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SettingsForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  okBtn;
	protected: 
	private: System::Windows::Forms::Button^  closeBtn;

	private:

		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(SettingsForm::typeid));
			this->okBtn = (gcnew System::Windows::Forms::Button());
			this->closeBtn = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// okBtn
			// 
			this->okBtn->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->okBtn->Location = System::Drawing::Point(301, 243);
			this->okBtn->Name = L"okBtn";
			this->okBtn->Size = System::Drawing::Size(80, 27);
			this->okBtn->TabIndex = 0;
			this->okBtn->Text = L"&OK";
			this->okBtn->UseVisualStyleBackColor = true;
			this->okBtn->Click += gcnew System::EventHandler(this, &SettingsForm::okBtn_Click);
			// 
			// closeBtn
			// 
			this->closeBtn->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->closeBtn->Location = System::Drawing::Point(215, 243);
			this->closeBtn->Name = L"closeBtn";
			this->closeBtn->Size = System::Drawing::Size(80, 27);
			this->closeBtn->TabIndex = 0;
			this->closeBtn->Text = L"&Cancel";
			this->closeBtn->UseVisualStyleBackColor = true;
			this->closeBtn->Click += gcnew System::EventHandler(this, &SettingsForm::closeBtn_Click);
			// 
			// SettingsForm
			// 
			this->AcceptButton = this->okBtn;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->closeBtn;
			this->ClientSize = System::Drawing::Size(393, 282);
			this->Controls->Add(this->closeBtn);
			this->Controls->Add(this->okBtn);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"SettingsForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Advanced Window Hotkeys - Settings";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &SettingsForm::SettingsForm_FormClosed);
			this->Load += gcnew System::EventHandler(this, &SettingsForm::SettingsForm_Load);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void okBtn_Click(System::Object^  sender, System::EventArgs^  e) {


				 //
				 // TODO: save the configuration
				 //


				 this->Close();
			 }
	private: System::Void closeBtn_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }
	private: System::Void SettingsForm_Load(System::Object^  sender, System::EventArgs^  e) {

				 //
				 // TODO: load the configuration
				 //


			 }
	private: System::Void SettingsForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			 }
};
}
