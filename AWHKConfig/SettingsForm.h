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
	private: System::Windows::Forms::CheckBox^  chkAllowSnap;
	private: System::Windows::Forms::ComboBox^  cmbGridX;
	private: System::Windows::Forms::ComboBox^  cmbGridY;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::CheckBox^  chkAdjacency;



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
			this->chkAllowSnap = (gcnew System::Windows::Forms::CheckBox());
			this->cmbGridX = (gcnew System::Windows::Forms::ComboBox());
			this->cmbGridY = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->chkAdjacency = (gcnew System::Windows::Forms::CheckBox());
			this->SuspendLayout();
			// 
			// okBtn
			// 
			this->okBtn->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->okBtn->Location = System::Drawing::Point(224, 167);
			this->okBtn->Name = L"okBtn";
			this->okBtn->Size = System::Drawing::Size(80, 27);
			this->okBtn->TabIndex = 1;
			this->okBtn->Text = L"&OK";
			this->okBtn->UseVisualStyleBackColor = true;
			this->okBtn->Click += gcnew System::EventHandler(this, &SettingsForm::okBtn_Click);
			// 
			// closeBtn
			// 
			this->closeBtn->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->closeBtn->Location = System::Drawing::Point(138, 167);
			this->closeBtn->Name = L"closeBtn";
			this->closeBtn->Size = System::Drawing::Size(80, 27);
			this->closeBtn->TabIndex = 0;
			this->closeBtn->Text = L"&Cancel";
			this->closeBtn->UseVisualStyleBackColor = true;
			this->closeBtn->Click += gcnew System::EventHandler(this, &SettingsForm::closeBtn_Click);
			// 
			// chkAllowSnap
			// 
			this->chkAllowSnap->AutoSize = true;
			this->chkAllowSnap->Location = System::Drawing::Point(12, 32);
			this->chkAllowSnap->Name = L"chkAllowSnap";
			this->chkAllowSnap->Size = System::Drawing::Size(188, 17);
			this->chkAllowSnap->TabIndex = 2;
			this->chkAllowSnap->Text = L"Enable snapping to other windows";
			this->chkAllowSnap->UseVisualStyleBackColor = true;
			// 
			// cmbGridX
			// 
			this->cmbGridX->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbGridX->FormattingEnabled = true;
			this->cmbGridX->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"4", L"8", L"16"});
			this->cmbGridX->Location = System::Drawing::Point(12, 90);
			this->cmbGridX->Name = L"cmbGridX";
			this->cmbGridX->Size = System::Drawing::Size(75, 21);
			this->cmbGridX->TabIndex = 3;
			// 
			// cmbGridY
			// 
			this->cmbGridY->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbGridY->FormattingEnabled = true;
			this->cmbGridY->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"4", L"8", L"16"});
			this->cmbGridY->Location = System::Drawing::Point(12, 117);
			this->cmbGridY->Name = L"cmbGridY";
			this->cmbGridY->Size = System::Drawing::Size(75, 21);
			this->cmbGridY->TabIndex = 3;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(93, 93);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(127, 13);
			this->label1->TabIndex = 4;
			this->label1->Text = L"snap columns per monitor";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(93, 120);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(110, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"snap rows per monitor";
			// 
			// chkAdjacency
			// 
			this->chkAdjacency->AutoSize = true;
			this->chkAdjacency->Location = System::Drawing::Point(12, 55);
			this->chkAdjacency->Name = L"chkAdjacency";
			this->chkAdjacency->Size = System::Drawing::Size(269, 17);
			this->chkAdjacency->TabIndex = 2;
			this->chkAdjacency->Text = L"Detect windows adjacent to the foreground window";
			this->chkAdjacency->UseVisualStyleBackColor = true;
			// 
			// SettingsForm
			// 
			this->AcceptButton = this->okBtn;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->closeBtn;
			this->ClientSize = System::Drawing::Size(316, 210);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->cmbGridY);
			this->Controls->Add(this->cmbGridX);
			this->Controls->Add(this->chkAdjacency);
			this->Controls->Add(this->chkAllowSnap);
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
			this->PerformLayout();

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
