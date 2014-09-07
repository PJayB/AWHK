/*
	Copyright (C) 2013 Peter J. B. Lewis

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
    and associated documentation files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge, publish, distribute, 
    sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or 
    substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
    BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "..\AWHKShared\Config.h"
#include "..\AWHKShared\IPC.h"
#include "..\AWHKShared\SupportFile.h"
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
	private: System::Windows::Forms::Panel^  controlContainer;
	protected: 

	protected: 
	private: System::Windows::Forms::Button^  btnAccept;
	private: System::Windows::Forms::CheckBox^  chkRunLogin;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label1;




	private: System::Windows::Forms::ComboBox^  cmbFineY;
	private: System::Windows::Forms::ComboBox^  cmbFineX;
	private: System::Windows::Forms::ComboBox^  cmbGridY;
	private: System::Windows::Forms::ComboBox^  cmbGridX;

	private: System::Windows::Forms::CheckBox^  chkAllowSnap;
	private: System::Windows::Forms::Button^  closeBtn;
	private: System::Windows::Forms::Button^  okBtn;
	private: System::Windows::Forms::Button^  quitBtn;
	private: System::Windows::Forms::CheckBox^  chkBaseWin;
	private: System::Windows::Forms::CheckBox^  chkBaseAlt;
	private: System::Windows::Forms::CheckBox^  chkBaseCtrl;











	private: System::Windows::Forms::CheckBox^  chkBaseShift;

	private: System::Windows::Forms::CheckBox^  chkAdjCtrl;
	private: System::Windows::Forms::CheckBox^  chkAdjAlt;


	private: System::Windows::Forms::CheckBox^  chkFineCtrl;
	private: System::Windows::Forms::CheckBox^  chkFineAlt;


	private: System::Windows::Forms::CheckBox^  chkAdjShift;

	private: System::Windows::Forms::CheckBox^  chkFineShift;
	private: System::Windows::Forms::CheckBox^  chkAdjWin;


	private: System::Windows::Forms::CheckBox^  chkFineWin;
	private: System::Windows::Forms::CheckBox^  chkAdjDisabled;

	private: System::Windows::Forms::CheckBox^  chkFineDisabled;





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
			this->controlContainer = (gcnew System::Windows::Forms::Panel());
			this->btnAccept = (gcnew System::Windows::Forms::Button());
			this->chkAdjCtrl = (gcnew System::Windows::Forms::CheckBox());
			this->chkAdjAlt = (gcnew System::Windows::Forms::CheckBox());
			this->chkFineCtrl = (gcnew System::Windows::Forms::CheckBox());
			this->chkFineAlt = (gcnew System::Windows::Forms::CheckBox());
			this->chkAdjShift = (gcnew System::Windows::Forms::CheckBox());
			this->chkAdjDisabled = (gcnew System::Windows::Forms::CheckBox());
			this->chkFineDisabled = (gcnew System::Windows::Forms::CheckBox());
			this->chkBaseCtrl = (gcnew System::Windows::Forms::CheckBox());
			this->chkFineShift = (gcnew System::Windows::Forms::CheckBox());
			this->chkAdjWin = (gcnew System::Windows::Forms::CheckBox());
			this->chkBaseAlt = (gcnew System::Windows::Forms::CheckBox());
			this->chkFineWin = (gcnew System::Windows::Forms::CheckBox());
			this->chkBaseShift = (gcnew System::Windows::Forms::CheckBox());
			this->chkBaseWin = (gcnew System::Windows::Forms::CheckBox());
			this->chkRunLogin = (gcnew System::Windows::Forms::CheckBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->cmbFineY = (gcnew System::Windows::Forms::ComboBox());
			this->cmbFineX = (gcnew System::Windows::Forms::ComboBox());
			this->cmbGridY = (gcnew System::Windows::Forms::ComboBox());
			this->cmbGridX = (gcnew System::Windows::Forms::ComboBox());
			this->chkAllowSnap = (gcnew System::Windows::Forms::CheckBox());
			this->quitBtn = (gcnew System::Windows::Forms::Button());
			this->closeBtn = (gcnew System::Windows::Forms::Button());
			this->okBtn = (gcnew System::Windows::Forms::Button());
			this->controlContainer->SuspendLayout();
			this->SuspendLayout();
			// 
			// controlContainer
			// 
			this->controlContainer->BackColor = System::Drawing::SystemColors::Control;
			this->controlContainer->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->controlContainer->Controls->Add(this->btnAccept);
			this->controlContainer->Controls->Add(this->chkAdjCtrl);
			this->controlContainer->Controls->Add(this->chkAdjAlt);
			this->controlContainer->Controls->Add(this->chkFineCtrl);
			this->controlContainer->Controls->Add(this->chkFineAlt);
			this->controlContainer->Controls->Add(this->chkAdjShift);
			this->controlContainer->Controls->Add(this->chkAdjDisabled);
			this->controlContainer->Controls->Add(this->chkFineDisabled);
			this->controlContainer->Controls->Add(this->chkBaseCtrl);
			this->controlContainer->Controls->Add(this->chkFineShift);
			this->controlContainer->Controls->Add(this->chkAdjWin);
			this->controlContainer->Controls->Add(this->chkBaseAlt);
			this->controlContainer->Controls->Add(this->chkFineWin);
			this->controlContainer->Controls->Add(this->chkBaseShift);
			this->controlContainer->Controls->Add(this->chkBaseWin);
			this->controlContainer->Controls->Add(this->chkRunLogin);
			this->controlContainer->Controls->Add(this->label7);
			this->controlContainer->Controls->Add(this->label6);
			this->controlContainer->Controls->Add(this->label5);
			this->controlContainer->Controls->Add(this->label4);
			this->controlContainer->Controls->Add(this->label2);
			this->controlContainer->Controls->Add(this->label3);
			this->controlContainer->Controls->Add(this->label1);
			this->controlContainer->Controls->Add(this->cmbFineY);
			this->controlContainer->Controls->Add(this->cmbFineX);
			this->controlContainer->Controls->Add(this->cmbGridY);
			this->controlContainer->Controls->Add(this->cmbGridX);
			this->controlContainer->Controls->Add(this->chkAllowSnap);
			this->controlContainer->Controls->Add(this->quitBtn);
			this->controlContainer->Controls->Add(this->closeBtn);
			this->controlContainer->Controls->Add(this->okBtn);
			this->controlContainer->Location = System::Drawing::Point(9, 9);
			this->controlContainer->Margin = System::Windows::Forms::Padding(0);
			this->controlContainer->Name = L"controlContainer";
			this->controlContainer->Size = System::Drawing::Size(440, 412);
			this->controlContainer->TabIndex = 0;
			// 
			// btnAccept
			// 
			this->btnAccept->Location = System::Drawing::Point(261, 371);
			this->btnAccept->Name = L"btnAccept";
			this->btnAccept->Size = System::Drawing::Size(80, 27);
			this->btnAccept->TabIndex = 27;
			this->btnAccept->Text = L"&Apply";
			this->btnAccept->UseVisualStyleBackColor = true;
			this->btnAccept->Click += gcnew System::EventHandler(this, &SettingsForm::btnAccept_Click);
			// 
			// chkAdjCtrl
			// 
			this->chkAdjCtrl->Checked = true;
			this->chkAdjCtrl->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chkAdjCtrl->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkAdjCtrl->Location = System::Drawing::Point(287, 157);
			this->chkAdjCtrl->Name = L"chkAdjCtrl";
			this->chkAdjCtrl->Size = System::Drawing::Size(50, 29);
			this->chkAdjCtrl->TabIndex = 26;
			this->chkAdjCtrl->Text = L"Ctrl";
			this->chkAdjCtrl->UseVisualStyleBackColor = true;
			this->chkAdjCtrl->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkAdjAlt
			// 
			this->chkAdjAlt->Checked = true;
			this->chkAdjAlt->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chkAdjAlt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkAdjAlt->Location = System::Drawing::Point(175, 157);
			this->chkAdjAlt->Name = L"chkAdjAlt";
			this->chkAdjAlt->Size = System::Drawing::Size(50, 29);
			this->chkAdjAlt->TabIndex = 26;
			this->chkAdjAlt->Text = L"Alt";
			this->chkAdjAlt->UseVisualStyleBackColor = true;
			this->chkAdjAlt->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkFineCtrl
			// 
			this->chkFineCtrl->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkFineCtrl->Location = System::Drawing::Point(287, 122);
			this->chkFineCtrl->Name = L"chkFineCtrl";
			this->chkFineCtrl->Size = System::Drawing::Size(50, 29);
			this->chkFineCtrl->TabIndex = 26;
			this->chkFineCtrl->Text = L"Ctrl";
			this->chkFineCtrl->UseVisualStyleBackColor = true;
			this->chkFineCtrl->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkFineAlt
			// 
			this->chkFineAlt->Checked = true;
			this->chkFineAlt->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chkFineAlt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkFineAlt->Location = System::Drawing::Point(175, 122);
			this->chkFineAlt->Name = L"chkFineAlt";
			this->chkFineAlt->Size = System::Drawing::Size(50, 29);
			this->chkFineAlt->TabIndex = 26;
			this->chkFineAlt->Text = L"Alt";
			this->chkFineAlt->UseVisualStyleBackColor = true;
			this->chkFineAlt->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkAdjShift
			// 
			this->chkAdjShift->Font = (gcnew System::Drawing::Font(L"Wingdings", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkAdjShift->Location = System::Drawing::Point(231, 157);
			this->chkAdjShift->Name = L"chkAdjShift";
			this->chkAdjShift->Size = System::Drawing::Size(50, 29);
			this->chkAdjShift->TabIndex = 26;
			this->chkAdjShift->Text = L"ñ";
			this->chkAdjShift->UseVisualStyleBackColor = true;
			this->chkAdjShift->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkAdjDisabled
			// 
			this->chkAdjDisabled->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkAdjDisabled->Location = System::Drawing::Point(343, 157);
			this->chkAdjDisabled->Name = L"chkAdjDisabled";
			this->chkAdjDisabled->Size = System::Drawing::Size(84, 29);
			this->chkAdjDisabled->TabIndex = 26;
			this->chkAdjDisabled->Text = L"Disable";
			this->chkAdjDisabled->UseVisualStyleBackColor = true;
			this->chkAdjDisabled->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::checkBox1_CheckedChanged);
			// 
			// chkFineDisabled
			// 
			this->chkFineDisabled->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkFineDisabled->Location = System::Drawing::Point(343, 122);
			this->chkFineDisabled->Name = L"chkFineDisabled";
			this->chkFineDisabled->Size = System::Drawing::Size(84, 29);
			this->chkFineDisabled->TabIndex = 26;
			this->chkFineDisabled->Text = L"Disable";
			this->chkFineDisabled->UseVisualStyleBackColor = true;
			this->chkFineDisabled->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineDisabled_CheckedChanged);
			// 
			// chkBaseCtrl
			// 
			this->chkBaseCtrl->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkBaseCtrl->Location = System::Drawing::Point(287, 87);
			this->chkBaseCtrl->Name = L"chkBaseCtrl";
			this->chkBaseCtrl->Size = System::Drawing::Size(50, 29);
			this->chkBaseCtrl->TabIndex = 26;
			this->chkBaseCtrl->Text = L"Ctrl";
			this->chkBaseCtrl->UseVisualStyleBackColor = true;
			this->chkBaseCtrl->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkFineShift
			// 
			this->chkFineShift->Checked = true;
			this->chkFineShift->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chkFineShift->Font = (gcnew System::Drawing::Font(L"Wingdings", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkFineShift->Location = System::Drawing::Point(231, 122);
			this->chkFineShift->Name = L"chkFineShift";
			this->chkFineShift->Size = System::Drawing::Size(50, 29);
			this->chkFineShift->TabIndex = 26;
			this->chkFineShift->Text = L"ñ";
			this->chkFineShift->UseVisualStyleBackColor = true;
			this->chkFineShift->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkAdjWin
			// 
			this->chkAdjWin->Font = (gcnew System::Drawing::Font(L"Wingdings", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkAdjWin->Location = System::Drawing::Point(119, 157);
			this->chkAdjWin->Name = L"chkAdjWin";
			this->chkAdjWin->Size = System::Drawing::Size(50, 29);
			this->chkAdjWin->TabIndex = 26;
			this->chkAdjWin->Text = L"ÿ";
			this->chkAdjWin->UseVisualStyleBackColor = true;
			this->chkAdjWin->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkBaseAlt
			// 
			this->chkBaseAlt->Checked = true;
			this->chkBaseAlt->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chkBaseAlt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkBaseAlt->Location = System::Drawing::Point(175, 87);
			this->chkBaseAlt->Name = L"chkBaseAlt";
			this->chkBaseAlt->Size = System::Drawing::Size(50, 29);
			this->chkBaseAlt->TabIndex = 26;
			this->chkBaseAlt->Text = L"Alt";
			this->chkBaseAlt->UseVisualStyleBackColor = true;
			this->chkBaseAlt->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkFineWin
			// 
			this->chkFineWin->Font = (gcnew System::Drawing::Font(L"Wingdings", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkFineWin->Location = System::Drawing::Point(119, 122);
			this->chkFineWin->Name = L"chkFineWin";
			this->chkFineWin->Size = System::Drawing::Size(50, 29);
			this->chkFineWin->TabIndex = 26;
			this->chkFineWin->Text = L"ÿ";
			this->chkFineWin->UseVisualStyleBackColor = true;
			this->chkFineWin->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkBaseShift
			// 
			this->chkBaseShift->Font = (gcnew System::Drawing::Font(L"Wingdings", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkBaseShift->Location = System::Drawing::Point(231, 87);
			this->chkBaseShift->Name = L"chkBaseShift";
			this->chkBaseShift->Size = System::Drawing::Size(50, 29);
			this->chkBaseShift->TabIndex = 26;
			this->chkBaseShift->Text = L"ñ";
			this->chkBaseShift->UseVisualStyleBackColor = true;
			this->chkBaseShift->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkBaseWin
			// 
			this->chkBaseWin->Font = (gcnew System::Drawing::Font(L"Wingdings", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chkBaseWin->Location = System::Drawing::Point(119, 87);
			this->chkBaseWin->Name = L"chkBaseWin";
			this->chkBaseWin->Size = System::Drawing::Size(50, 29);
			this->chkBaseWin->TabIndex = 26;
			this->chkBaseWin->Text = L"ÿ";
			this->chkBaseWin->UseVisualStyleBackColor = true;
			this->chkBaseWin->CheckedChanged += gcnew System::EventHandler(this, &SettingsForm::chkFineWin_CheckedChanged);
			// 
			// chkRunLogin
			// 
			this->chkRunLogin->AutoSize = true;
			this->chkRunLogin->Location = System::Drawing::Point(12, 42);
			this->chkRunLogin->Name = L"chkRunLogin";
			this->chkRunLogin->Size = System::Drawing::Size(150, 17);
			this->chkRunLogin->TabIndex = 26;
			this->chkRunLogin->Text = L"Run when Windows starts";
			this->chkRunLogin->UseVisualStyleBackColor = true;
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(9, 164);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(101, 13);
			this->label7->TabIndex = 25;
			this->label7->Text = L"Layout Preservation";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(57, 129);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(55, 13);
			this->label6->TabIndex = 24;
			this->label6->Text = L"Fine-Snap";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(34, 94);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(76, 13);
			this->label5->TabIndex = 23;
			this->label5->Text = L"Base Modifiers";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(93, 308);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(130, 13);
			this->label4->TabIndex = 21;
			this->label4->Text = L"fine-snap rows per monitor";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(93, 244);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(110, 13);
			this->label2->TabIndex = 20;
			this->label2->Text = L"snap rows per monitor";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(93, 281);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(147, 13);
			this->label3->TabIndex = 19;
			this->label3->Text = L"fine-snap columns per monitor";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(93, 217);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(127, 13);
			this->label1->TabIndex = 22;
			this->label1->Text = L"snap columns per monitor";
			// 
			// cmbFineY
			// 
			this->cmbFineY->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbFineY->FormattingEnabled = true;
			this->cmbFineY->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"4", L"8", L"16", L"32", L"64", L"128"});
			this->cmbFineY->Location = System::Drawing::Point(12, 305);
			this->cmbFineY->Name = L"cmbFineY";
			this->cmbFineY->Size = System::Drawing::Size(75, 21);
			this->cmbFineY->TabIndex = 15;
			// 
			// cmbFineX
			// 
			this->cmbFineX->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbFineX->FormattingEnabled = true;
			this->cmbFineX->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"4", L"8", L"16", L"32", L"64", L"128"});
			this->cmbFineX->Location = System::Drawing::Point(12, 278);
			this->cmbFineX->Name = L"cmbFineX";
			this->cmbFineX->Size = System::Drawing::Size(75, 21);
			this->cmbFineX->TabIndex = 14;
			// 
			// cmbGridY
			// 
			this->cmbGridY->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbGridY->FormattingEnabled = true;
			this->cmbGridY->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"4", L"8", L"16"});
			this->cmbGridY->Location = System::Drawing::Point(12, 241);
			this->cmbGridY->Name = L"cmbGridY";
			this->cmbGridY->Size = System::Drawing::Size(75, 21);
			this->cmbGridY->TabIndex = 13;
			// 
			// cmbGridX
			// 
			this->cmbGridX->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbGridX->FormattingEnabled = true;
			this->cmbGridX->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"4", L"8", L"16"});
			this->cmbGridX->Location = System::Drawing::Point(12, 214);
			this->cmbGridX->Name = L"cmbGridX";
			this->cmbGridX->Size = System::Drawing::Size(75, 21);
			this->cmbGridX->TabIndex = 12;
			// 
			// chkAllowSnap
			// 
			this->chkAllowSnap->AutoSize = true;
			this->chkAllowSnap->Location = System::Drawing::Point(12, 19);
			this->chkAllowSnap->Name = L"chkAllowSnap";
			this->chkAllowSnap->Size = System::Drawing::Size(188, 17);
			this->chkAllowSnap->TabIndex = 10;
			this->chkAllowSnap->Text = L"Enable snapping to other windows";
			this->chkAllowSnap->UseVisualStyleBackColor = true;
			// 
			// quitBtn
			// 
			this->quitBtn->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->quitBtn->Location = System::Drawing::Point(12, 371);
			this->quitBtn->Name = L"quitBtn";
			this->quitBtn->Size = System::Drawing::Size(80, 27);
			this->quitBtn->TabIndex = 8;
			this->quitBtn->Text = L"&Quit";
			this->quitBtn->UseVisualStyleBackColor = true;
			this->quitBtn->Click += gcnew System::EventHandler(this, &SettingsForm::quitBtn_Click);
			// 
			// closeBtn
			// 
			this->closeBtn->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->closeBtn->Location = System::Drawing::Point(347, 371);
			this->closeBtn->Name = L"closeBtn";
			this->closeBtn->Size = System::Drawing::Size(80, 27);
			this->closeBtn->TabIndex = 8;
			this->closeBtn->Text = L"&Cancel";
			this->closeBtn->UseVisualStyleBackColor = true;
			this->closeBtn->Click += gcnew System::EventHandler(this, &SettingsForm::closeBtn_Click);
			// 
			// okBtn
			// 
			this->okBtn->Location = System::Drawing::Point(175, 371);
			this->okBtn->Name = L"okBtn";
			this->okBtn->Size = System::Drawing::Size(80, 27);
			this->okBtn->TabIndex = 9;
			this->okBtn->Text = L"&OK";
			this->okBtn->UseVisualStyleBackColor = true;
			this->okBtn->Click += gcnew System::EventHandler(this, &SettingsForm::okBtn_Click);
			// 
			// SettingsForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ControlDark;
			this->ClientSize = System::Drawing::Size(458, 430);
			this->Controls->Add(this->controlContainer);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(474, 469);
			this->Name = L"SettingsForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Advanced Window Hotkeys - Settings";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &SettingsForm::SettingsForm_FormClosed);
			this->Load += gcnew System::EventHandler(this, &SettingsForm::SettingsForm_Load);
			this->Shown += gcnew System::EventHandler(this, &SettingsForm::SettingsForm_Shown);
			this->Resize += gcnew System::EventHandler(this, &SettingsForm::SettingsForm_Resize);
			this->controlContainer->ResumeLayout(false);
			this->controlContainer->PerformLayout();
			this->ResumeLayout(false);

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

		DWORD GetBaseModifiers()
		{
			return 
				( chkBaseWin->Checked ? MOD_WIN : 0 ) |
				( chkBaseAlt->Checked ? MOD_ALT : 0 ) |
				( chkBaseShift->Checked ? MOD_SHIFT : 0 ) |
				( chkBaseCtrl->Checked ? MOD_CONTROL : 0 );
		}

		DWORD GetAdjModifiers()
		{
			if ( chkAdjDisabled->Checked )
				return 0;

			return 
				( chkAdjWin->Checked ? MOD_WIN : 0 ) |
				( chkAdjAlt->Checked ? MOD_ALT : 0 ) |
				( chkAdjShift->Checked ? MOD_SHIFT : 0 ) |
				( chkAdjCtrl->Checked ? MOD_CONTROL : 0 );
		}

		DWORD GetFineModifiers()
		{
			if ( chkFineDisabled->Checked )
				return 0;

			return 
				( chkFineWin->Checked ? MOD_WIN : 0 ) |
				( chkFineAlt->Checked ? MOD_ALT : 0 ) |
				( chkFineShift->Checked ? MOD_SHIFT : 0 ) |
				( chkFineCtrl->Checked ? MOD_CONTROL : 0 );
		}

		bool SaveConfig()
		{
			DWORD dwBaseMods = GetBaseModifiers();
			if ( !dwBaseMods )
			{
				MessageBox::Show(
					"You must check at least one Base Modifier.",
					"Advanced Window HotKeys",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error );

				return false;
			}

			if ( dwBaseMods == MOD_WIN )
			{
				MessageBox::Show(
					"The Windows key is reserved by the system. Please add an additional modifier.",
					"Advanced Window HotKeys",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error );

				return false;
			}

			DWORD dwAdjMods = GetAdjModifiers();
			DWORD dwFineMods = GetFineModifiers();

			if ( dwAdjMods != 0 && dwAdjMods == dwFineMods )
			{
				MessageBox::Show(
					"The Fine-Snap and Layout Preservation modifiers are the same.",
					"Advanced Window HotKeys",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error );

				return false;
			}

			if ( dwAdjMods == dwBaseMods ||
				 dwFineMods == dwBaseMods )
			{
				if ( MessageBox::Show(
					"One of the extra modifiers are the same as the base. These will be disabled.",
					"Advanced Window HotKeys",
					MessageBoxButtons::OKCancel,
					MessageBoxIcon::Information ) == Windows::Forms::DialogResult::Cancel )
				{
					return false;
				}

				if ( dwAdjMods == dwBaseMods ) 
				{ 
					chkAdjDisabled->Checked = true;
					dwAdjMods = 0; 
				}
				if ( dwFineMods == dwBaseMods )
				{ 
					chkFineDisabled->Checked = true;
					dwFineMods = 0; 
				}				
			}

			//
			// save the configuration
			//
			AWHK_APP_CONFIG cfg;

			cfg.AllowSnapToOthers = chkAllowSnap->Checked;
			//cfg.AllowModifyAdjacent = chkAdjacency->Checked;

			cfg.GridX = StringToDword( cmbGridX->Text );
			cfg.GridY = StringToDword( cmbGridY->Text );
			cfg.FineX = StringToDword( cmbFineX->Text );
			cfg.FineY = StringToDword( cmbFineY->Text );

			// TODO: validate these!
			cfg.MoveKeyMod = dwBaseMods;
			cfg.NextKeyMod = dwAdjMods;
			cfg.FineKeyMod = dwFineMods;

			SaveConfiguration( &cfg );

			SetAutoLoginEnabled( chkRunLogin->Checked );

			return true;
		}

	private: System::Void okBtn_Click(System::Object^  sender, System::EventArgs^  e) {

				 if ( SaveConfig() )
				 {
					this->DialogResult = System::Windows::Forms::DialogResult::OK;
					this->Close();
				 }
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

	void CheckBaseFromMods( DWORD mods )
	{
		chkBaseWin->Checked		= ( mods & MOD_WIN ) != 0;
		chkBaseAlt->Checked		= ( mods & MOD_ALT ) != 0;
		chkBaseShift->Checked	= ( mods & MOD_SHIFT ) != 0;
		chkBaseCtrl->Checked	= ( mods & MOD_CONTROL ) != 0;
	}

	void CheckAdjFromMods( DWORD mods )
	{
		if ( !mods )
			chkAdjDisabled->Checked = true;
		else
		{
			chkAdjWin->Checked		= ( mods & MOD_WIN ) != 0;
			chkAdjAlt->Checked		= ( mods & MOD_ALT ) != 0;
			chkAdjShift->Checked	= ( mods & MOD_SHIFT ) != 0;
			chkAdjCtrl->Checked		= ( mods & MOD_CONTROL ) != 0;
		}
	}

	void CheckFineFromMods( DWORD mods )
	{
		if ( !mods )
			chkFineDisabled->Checked = true;
		else
		{
			chkFineWin->Checked		= ( mods & MOD_WIN ) != 0;
			chkFineAlt->Checked		= ( mods & MOD_ALT ) != 0;
			chkFineShift->Checked	= ( mods & MOD_SHIFT ) != 0;
			chkFineCtrl->Checked	= ( mods & MOD_CONTROL ) != 0;
		}
	}

    BOOL RunAWHK()
    {
        WCHAR strExe[MAX_PATH];
		if ( !GetSupportFilePath( AWHK_MAIN_EXE, strExe, MAX_PATH ) )
            return FALSE;

        PROCESS_INFORMATION pi;
        ZeroMemory( &pi, sizeof( pi ) );
	                    
        STARTUPINFO startInfo;
	    ZeroMemory( &startInfo, sizeof( startInfo ) );
	    startInfo.cb = sizeof( startInfo );

        // Start the process
        return ::CreateProcess(
	        nullptr,
	        strExe,
	        nullptr,
	        nullptr,
	        FALSE,
	        0,
	        nullptr,
	        nullptr,
	        &startInfo,
	        &pi );
    }

	private: System::Void SettingsForm_Load(System::Object^  sender, System::EventArgs^  e) {

                 // Try and run the app
				 IPC ipc;
				 if ( !OpenIPC( &ipc ) )
				 {
                     RunAWHK();
				 }
                 else 
                 {
                     CloseIPC( &ipc );
                 }

                 //
				 // load the configuration
				 //
				 AWHK_APP_CONFIG cfg;
				 LoadConfiguration( &cfg );

				 chkAllowSnap->Checked = ( cfg.AllowSnapToOthers != 0 );
				 //chkAdjacency->Checked = ( cfg.AllowModifyAdjacent != 0 );

				 LoadGridValue( cfg.GridX, cmbGridX );
				 LoadGridValue( cfg.GridY, cmbGridY );
				 LoadGridValue( cfg.FineX, cmbFineX );
				 LoadGridValue( cfg.FineY, cmbFineY );

				 CheckBaseFromMods( cfg.MoveKeyMod );
				 CheckAdjFromMods( cfg.NextKeyMod );
				 CheckFineFromMods( cfg.FineKeyMod );

				 chkRunLogin->Checked = ( IsAutoLoginEnabled() != FALSE );
			 }
	private: System::Void SettingsForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			 }
private: System::Void SettingsForm_Shown(System::Object^  sender, System::EventArgs^  e) {
				 okBtn->Focus();
		 }
private: System::Void btnAccept_Click(System::Object^  sender, System::EventArgs^  e) {

			 if ( SaveConfig() )
			 {
				 BOOL didItWork = FALSE;

				 IPC ipc;
				 if ( OpenIPC( &ipc ) )
				 {
					 didItWork = WriteMessageIPC( &ipc, IPC_MSG_RELOAD_CONFIG );
				 }
			  
				 if ( !didItWork )
				 {
					 MessageBox::Show(
						 this,
						 "It appears AWHK is not running. Changes will be applied when the application is next run.",
						 "Advanced Windows HotKeys",
						 MessageBoxButtons::OK,
						 MessageBoxIcon::Information );
				 }

				 CloseIPC( &ipc );
			 }
		 }
private: System::Void SettingsForm_Resize(System::Object^  sender, System::EventArgs^  e) {
			 controlContainer->Left = (this->ClientSize.Width  - controlContainer->Width) / 2;
			 controlContainer->Top  = (this->ClientSize.Height - controlContainer->Height) / 2;
		 }
private: System::Void quitBtn_Click(System::Object^  sender, System::EventArgs^  e) {

			 IPC ipc;
			 if ( !OpenIPC( &ipc ) )
			 {
				 MessageBox::Show(
					 this,
					 "It appears AWHK is not running.",
					 "Advanced Windows HotKeys",
					 MessageBoxButtons::OK,
					 MessageBoxIcon::Information );
				 return;
			 }
			  
			 if ( MessageBox::Show(
				 this,
				 "Are you sure you want to unload AWHK?",
				 "Quit Prompt",
				 MessageBoxButtons::YesNo,
				 MessageBoxIcon::Question ) != Windows::Forms::DialogResult::Yes )
			 {
				 return;
			 }

			 WriteMessageIPC( &ipc, IPC_MSG_QUIT );
			 CloseIPC( &ipc );
		 }

private: System::Void chkFineWin_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {

			DWORD dwMoveKeyMod = GetBaseModifiers();
			DWORD dwNextKeyMod = GetAdjModifiers();
			DWORD dwFineKeyMod = GetFineModifiers();

			CheckBaseFromMods(	dwMoveKeyMod );
			CheckAdjFromMods(	dwMoveKeyMod | dwNextKeyMod );
			CheckFineFromMods(	dwMoveKeyMod | dwFineKeyMod );

		 }

		 void EnableAndRecheck( CheckBox^ chk, bool enable )
		 {
			 chk->Enabled = enable;
			 chkFineWin_CheckedChanged(chk, nullptr);
		 }


private: System::Void chkFineDisabled_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {

			 CheckBox^ chk = (CheckBox^) sender;
			 EnableAndRecheck( chkFineWin,		!chk->Checked );
			 EnableAndRecheck( chkFineAlt,		!chk->Checked );
			 EnableAndRecheck( chkFineShift,	!chk->Checked );
			 EnableAndRecheck( chkFineCtrl,		!chk->Checked );
		 }
private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {

			 CheckBox^ chk = (CheckBox^) sender;
			 EnableAndRecheck( chkAdjWin,		!chk->Checked );
			 EnableAndRecheck( chkAdjAlt,		!chk->Checked );
			 EnableAndRecheck( chkAdjShift,		!chk->Checked );
			 EnableAndRecheck( chkAdjCtrl,		!chk->Checked );
		 }
};
}
