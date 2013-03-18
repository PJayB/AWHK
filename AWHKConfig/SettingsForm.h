#pragma once

#include "..\AWHKShared\Config.h"
#include "..\AWHKShared\IPC.h"
#include "AutoLogin.h"

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
	private: System::Windows::Forms::CheckBox^  chkAdjacency;
	private: System::Windows::Forms::ComboBox^  cmbGridX;
	private: System::Windows::Forms::ComboBox^  cmbGridY;
	private: System::Windows::Forms::ComboBox^  cmbFineX;
	private: System::Windows::Forms::ComboBox^  cmbFineY;
	private: System::Windows::Forms::ComboBox^  cmbMoveMod;
	private: System::Windows::Forms::ComboBox^  cmbFineMod;
	private: System::Windows::Forms::ComboBox^  cmbSoloMod;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::CheckBox^  chkRunLogin;
	private: System::Windows::Forms::Button^  btnAccept;

	protected: 



























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
			this->chkAdjacency = (gcnew System::Windows::Forms::CheckBox());
			this->cmbGridX = (gcnew System::Windows::Forms::ComboBox());
			this->cmbGridY = (gcnew System::Windows::Forms::ComboBox());
			this->cmbFineX = (gcnew System::Windows::Forms::ComboBox());
			this->cmbFineY = (gcnew System::Windows::Forms::ComboBox());
			this->cmbMoveMod = (gcnew System::Windows::Forms::ComboBox());
			this->cmbFineMod = (gcnew System::Windows::Forms::ComboBox());
			this->cmbSoloMod = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->chkRunLogin = (gcnew System::Windows::Forms::CheckBox());
			this->btnAccept = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// okBtn
			// 
			this->okBtn->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->okBtn->Location = System::Drawing::Point(502, 257);
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
			this->closeBtn->Location = System::Drawing::Point(12, 257);
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
			this->chkAllowSnap->Enabled = false;
			this->chkAllowSnap->Location = System::Drawing::Point(12, 32);
			this->chkAllowSnap->Name = L"chkAllowSnap";
			this->chkAllowSnap->Size = System::Drawing::Size(188, 17);
			this->chkAllowSnap->TabIndex = 2;
			this->chkAllowSnap->Text = L"Enable snapping to other windows";
			this->chkAllowSnap->UseVisualStyleBackColor = true;
			// 
			// chkAdjacency
			// 
			this->chkAdjacency->AutoSize = true;
			this->chkAdjacency->Location = System::Drawing::Point(12, 55);
			this->chkAdjacency->Name = L"chkAdjacency";
			this->chkAdjacency->Size = System::Drawing::Size(264, 17);
			this->chkAdjacency->TabIndex = 2;
			this->chkAdjacency->Text = L"Move windows adjacent to the foreground window";
			this->chkAdjacency->UseVisualStyleBackColor = true;
			// 
			// cmbGridX
			// 
			this->cmbGridX->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbGridX->FormattingEnabled = true;
			this->cmbGridX->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"4", L"8", L"16"});
			this->cmbGridX->Location = System::Drawing::Point(335, 30);
			this->cmbGridX->Name = L"cmbGridX";
			this->cmbGridX->Size = System::Drawing::Size(75, 21);
			this->cmbGridX->TabIndex = 3;
			// 
			// cmbGridY
			// 
			this->cmbGridY->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbGridY->FormattingEnabled = true;
			this->cmbGridY->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"4", L"8", L"16"});
			this->cmbGridY->Location = System::Drawing::Point(335, 57);
			this->cmbGridY->Name = L"cmbGridY";
			this->cmbGridY->Size = System::Drawing::Size(75, 21);
			this->cmbGridY->TabIndex = 3;
			// 
			// cmbFineX
			// 
			this->cmbFineX->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbFineX->FormattingEnabled = true;
			this->cmbFineX->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"4", L"8", L"16", L"32", L"64", L"128"});
			this->cmbFineX->Location = System::Drawing::Point(335, 94);
			this->cmbFineX->Name = L"cmbFineX";
			this->cmbFineX->Size = System::Drawing::Size(75, 21);
			this->cmbFineX->TabIndex = 3;
			// 
			// cmbFineY
			// 
			this->cmbFineY->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbFineY->FormattingEnabled = true;
			this->cmbFineY->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"4", L"8", L"16", L"32", L"64", L"128"});
			this->cmbFineY->Location = System::Drawing::Point(335, 121);
			this->cmbFineY->Name = L"cmbFineY";
			this->cmbFineY->Size = System::Drawing::Size(75, 21);
			this->cmbFineY->TabIndex = 3;
			// 
			// cmbMoveMod
			// 
			this->cmbMoveMod->DisplayMember = L"0";
			this->cmbMoveMod->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbMoveMod->Enabled = false;
			this->cmbMoveMod->FormattingEnabled = true;
			this->cmbMoveMod->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Alt", L"Shift", L"Ctrl"});
			this->cmbMoveMod->Location = System::Drawing::Point(12, 126);
			this->cmbMoveMod->Name = L"cmbMoveMod";
			this->cmbMoveMod->Size = System::Drawing::Size(93, 21);
			this->cmbMoveMod->TabIndex = 3;
			// 
			// cmbFineMod
			// 
			this->cmbFineMod->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbFineMod->FormattingEnabled = true;
			this->cmbFineMod->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"(disabled)", L"Shift", L"Ctrl"});
			this->cmbFineMod->Location = System::Drawing::Point(12, 153);
			this->cmbFineMod->Name = L"cmbFineMod";
			this->cmbFineMod->Size = System::Drawing::Size(93, 21);
			this->cmbFineMod->TabIndex = 3;
			this->cmbFineMod->SelectedIndexChanged += gcnew System::EventHandler(this, &SettingsForm::cmbSoloMod_SelectedIndexChanged);
			// 
			// cmbSoloMod
			// 
			this->cmbSoloMod->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSoloMod->FormattingEnabled = true;
			this->cmbSoloMod->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"(disabled)", L"Shift", L"Ctrl"});
			this->cmbSoloMod->Location = System::Drawing::Point(12, 180);
			this->cmbSoloMod->Name = L"cmbSoloMod";
			this->cmbSoloMod->Size = System::Drawing::Size(93, 21);
			this->cmbSoloMod->TabIndex = 3;
			this->cmbSoloMod->SelectedIndexChanged += gcnew System::EventHandler(this, &SettingsForm::cmbSoloMod_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(416, 33);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(127, 13);
			this->label1->TabIndex = 4;
			this->label1->Text = L"snap columns per monitor";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(416, 97);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(147, 13);
			this->label3->TabIndex = 4;
			this->label3->Text = L"fine-snap columns per monitor";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(416, 60);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(110, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"snap rows per monitor";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(416, 124);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(130, 13);
			this->label4->TabIndex = 4;
			this->label4->Text = L"fine-snap rows per monitor";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(111, 129);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(90, 13);
			this->label5->TabIndex = 5;
			this->label5->Text = L"Base key modifier";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(111, 156);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(92, 13);
			this->label6->TabIndex = 5;
			this->label6->Text = L"Fine-snap modifier";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(111, 183);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(114, 13);
			this->label7->TabIndex = 5;
			this->label7->Text = L"Single window modifier";
			// 
			// chkRunLogin
			// 
			this->chkRunLogin->AutoSize = true;
			this->chkRunLogin->Location = System::Drawing::Point(12, 78);
			this->chkRunLogin->Name = L"chkRunLogin";
			this->chkRunLogin->Size = System::Drawing::Size(150, 17);
			this->chkRunLogin->TabIndex = 6;
			this->chkRunLogin->Text = L"Run when Windows starts";
			this->chkRunLogin->UseVisualStyleBackColor = true;
			// 
			// btnAccept
			// 
			this->btnAccept->Location = System::Drawing::Point(419, 257);
			this->btnAccept->Name = L"btnAccept";
			this->btnAccept->Size = System::Drawing::Size(80, 27);
			this->btnAccept->TabIndex = 7;
			this->btnAccept->Text = L"&Accept";
			this->btnAccept->UseVisualStyleBackColor = true;
			this->btnAccept->Click += gcnew System::EventHandler(this, &SettingsForm::btnAccept_Click);
			// 
			// SettingsForm
			// 
			this->AcceptButton = this->okBtn;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->closeBtn;
			this->ClientSize = System::Drawing::Size(594, 296);
			this->Controls->Add(this->btnAccept);
			this->Controls->Add(this->chkRunLogin);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->cmbSoloMod);
			this->Controls->Add(this->cmbFineMod);
			this->Controls->Add(this->cmbMoveMod);
			this->Controls->Add(this->cmbFineY);
			this->Controls->Add(this->cmbFineX);
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
			this->Shown += gcnew System::EventHandler(this, &SettingsForm::SettingsForm_Shown);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private:

		DWORD StringToDword( String^ str )
		{
			System::UInt32 i = System::UInt32::Parse( str );
			return (DWORD) i;
		}

		DWORD StringToKeyMod( String^ v )
		{
			if ( v == "Shift" )
				return MOD_SHIFT;
			else if ( v == "Alt" )
				return MOD_ALT;
			else if ( v == "Ctrl" )
				return MOD_CONTROL;
			else
				return 0;
		}

	private:

		void SaveConfig()
		{
			//
			// save the configuration
			//
			AWHK_APP_CONFIG cfg;

			cfg.AllowSnapToOthers = chkAllowSnap->Checked;
			cfg.AllowModifyAdjacent = chkAdjacency->Checked;

			cfg.GridX = StringToDword( cmbGridX->Text );
			cfg.GridY = StringToDword( cmbGridY->Text );
			cfg.FineX = StringToDword( cmbFineX->Text );
			cfg.FineY = StringToDword( cmbFineY->Text );

			// TODO: validate these!
			cfg.MoveKeyMod = StringToKeyMod( cmbMoveMod->Text );
			cfg.SoloKeyMod = StringToKeyMod( cmbSoloMod->Text );
			cfg.FineKeyMod = StringToKeyMod( cmbFineMod->Text );

			SaveConfiguration( &cfg );

			SetAutoLoginEnabled( chkRunLogin->Checked );
		}

	private: System::Void okBtn_Click(System::Object^  sender, System::EventArgs^  e) {

				 SaveConfig();

				 this->Close();
			 }
	private: System::Void closeBtn_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }

	Void LoadGridValue( UInt32 value, Windows::Forms::ComboBox^ combo )
	{
		String^ i = value.ToString();
		if ( !combo->Items->Contains( i ) )
			combo->Items->Add( i );

		combo->Text = i;
	}

	String^ KeyModToString( DWORD value, String^ def )
	{
		switch ( value )
		{
		case MOD_SHIFT:
			return "Shift";
		case MOD_ALT:
			return "Alt";
		case MOD_CONTROL:
			return "Ctrl";
		default:
			return def;
		}
	}

	private: System::Void SettingsForm_Load(System::Object^  sender, System::EventArgs^  e) {

				 //
				 // load the configuration
				 //
				 AWHK_APP_CONFIG cfg;
				 LoadConfiguration( &cfg );

				 chkAllowSnap->Checked = ( cfg.AllowSnapToOthers != 0 );
				 chkAdjacency->Checked = ( cfg.AllowModifyAdjacent != 0 );

				 LoadGridValue( cfg.GridX, cmbGridX );
				 LoadGridValue( cfg.GridY, cmbGridY );
				 LoadGridValue( cfg.FineX, cmbFineX );
				 LoadGridValue( cfg.FineY, cmbFineY );

				 cmbMoveMod->Text = KeyModToString( cfg.MoveKeyMod, cmbMoveMod->Items[0]->ToString() );
				 cmbSoloMod->Text = KeyModToString( cfg.SoloKeyMod, cmbSoloMod->Items[0]->ToString() );
				 cmbFineMod->Text = KeyModToString( cfg.FineKeyMod, cmbFineMod->Items[0]->ToString() );

				 chkRunLogin->Checked = ( IsAutoLoginEnabled() != FALSE );
			 }
	private: System::Void SettingsForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			 }
private: System::Void SettingsForm_Shown(System::Object^  sender, System::EventArgs^  e) {
				 okBtn->Focus();
		 }
private: System::Void cmbSoloMod_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ( cmbSoloMod->SelectedIndex == cmbFineMod->SelectedIndex &&
				  cmbSoloMod->SelectedIndex > 0 &&
				  cmbSoloMod->Items->Count == cmbFineMod->Items->Count )
			 {
				 int count = cmbFineMod->Items->Count - 1;
				 int newIndex = 1 + ( cmbFineMod->SelectedIndex ) % count;

				 if ( sender == cmbSoloMod )
					 cmbFineMod->SelectedIndex = newIndex;
				 else
					 cmbSoloMod->SelectedIndex = newIndex;
			 }
		 }
private: System::Void btnAccept_Click(System::Object^  sender, System::EventArgs^  e) {

			 SaveConfig();

			 BOOL didItWork = FALSE;

			 IPC ipc;
			 if ( OpenIPC( &ipc ) )
			 {
				 didItWork = WriteMessageIPC( &ipc, IPC_MSG_RELOAD_CONFIG );
			 }
			  
			 if ( !didItWork )
			 {
				 Windows::Forms::MessageBox::Show(
					 this,
					 "It appears AWHK is not running. Changes will be applied when the application is next run.",
					 "Advanced Windows HotKeys",
					 MessageBoxButtons::OK,
					 MessageBoxIcon::Information );
			 }
		 }
};
}
