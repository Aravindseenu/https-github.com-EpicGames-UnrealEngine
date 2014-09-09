// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once


#define LOCTEXT_NAMESPACE "SProjectLauncherSimpleDeviceListRow"


/**
 * Implements a row widget for the launcher's device proxy list.
 */
class SProjectLauncherSimpleDeviceListRow
	: public STableRow<ITargetDeviceProxyPtr>
{
public:

	SLATE_BEGIN_ARGS(SProjectLauncherSimpleDeviceListRow) { }
		/**
		* The Callback for when the launch button is clicked.
		*/
		SLATE_EVENT(FOnProfileRun, OnProfileRun)
	
		/**
		 * The device proxy shown in this row.
		 */
		SLATE_ARGUMENT(ITargetDeviceProxyPtr, DeviceProxy)

		/**
		 * Whether the advanced options should be shown.
		 */
		SLATE_ATTRIBUTE(bool, IsAdvanced)

	SLATE_END_ARGS()

public:

	~SProjectLauncherSimpleDeviceListRow()
	{
		if (LaunchProfile.IsValid())
		{
			Model->GetProfileManager()->RemoveProfile(LaunchProfile.ToSharedRef());
		}
	}

	/**
	 * Constructs the widget.
	 *
	 * @param InArgs - The construction arguments.
	 * @param InModel - The launcher model this list uses.
	 */
	void Construct(const FArguments& InArgs, const FProjectLauncherModelRef& InModel, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		STableRow< ITargetDeviceProxyPtr >::ConstructInternal(
			STableRow::FArguments()
			.ShowSelection(false),
			InOwnerTableView
			);

		Model = InModel;
		IsAdvanced = InArgs._IsAdvanced;
		OnProfileRun = InArgs._OnProfileRun;
		DeviceProxy = InArgs._DeviceProxy;

		SimpleProfile = Model->GetProfileManager()->FindOrAddSimpleProfile(DeviceProxy->GetName());

		LaunchProfile = Model->GetProfileManager()->CreateUnsavedProfile(DeviceProxy->GetName());
		UpdateProfile();

		TSharedRef<SUniformGridPanel> NameGrid = SNew(SUniformGridPanel).SlotPadding(FMargin(0.0f, 1.0f));
		TSharedRef<SUniformGridPanel> ValueGrid = SNew(SUniformGridPanel).SlotPadding(FMargin(0.0f, 1.0f));
		
		MakeAdvancedSettings(NameGrid, ValueGrid);

		ChildSlot
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1)
			.Padding(0,2,0,2)
			[
				SNew(SBorder)
				.Padding(2)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Top)
					[
						SNew(SBox)
						.WidthOverride(40)
						.HeightOverride(40)
						[
							SNew(SImage)
							.Image(this, &SProjectLauncherSimpleDeviceListRow::HandleDeviceImage)
						]
					]

					+ SHorizontalBox::Slot()
					.FillWidth(1)
					.VAlign(VAlign_Top)
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(2,4,2,4)
						[
							SNew(STextBlock)
							.Text(this, &SProjectLauncherSimpleDeviceListRow::HandleDeviceNameText)
						]

						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(2, 4, 2, 4)
						[
							SNew(STextBlock)
							.Text(this, &SProjectLauncherSimpleDeviceListRow::HandleHostPlatformText)
						]
					]

					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						// This Vertical box ensures the NameGrid spans only the vertical space the ValueGrid forces.
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0)
						[
							SNew(SHorizontalBox)

							+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(2, 0, 4, 0)
							[
								NameGrid
							]

							+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Top)
							[
								ValueGrid
							]
						]
					]

					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(4,0,0,0)
					[
						SNew(SProjectLauncherProfileLaunchButton, true)
						.LaunchProfile(this, &SProjectLauncherSimpleDeviceListRow::GetLaunchProfile)
						.OnClicked(this, &SProjectLauncherSimpleDeviceListRow::OnLaunchClicked)
					]
				]
			]
		];

	}

private:

	// Create simple settings

	void MakeSimpleSettings(TSharedRef<SUniformGridPanel> InNameColumn, TSharedRef<SUniformGridPanel> InValueColumn)
	{
		int Row = 0;
		if (DeviceProxy->CanSupportVariants())
		{
			MakeVariantRow(InNameColumn, InValueColumn, Row++);
		}
	}

	// Create Advanced settings

	void MakeAdvancedSettings(TSharedRef<SUniformGridPanel> InNameColumn, TSharedRef<SUniformGridPanel> InValueColumn)
	{
		int Row = 0;
		if (DeviceProxy->GetNumVariants() > 1)
		{
			MakeVariantRow(InNameColumn, InValueColumn, Row++);
		}
		MakeBuildConfigurationRow(InNameColumn, InValueColumn, Row++);
		MakeCookModeRow(InNameColumn, InValueColumn, Row++);
	}

	// Type Settings Row

	void MakeVariantRow(TSharedRef<SUniformGridPanel> InNameColumn, TSharedRef<SUniformGridPanel> InValueColumn, int InRowIndex)
	{
		InNameColumn->AddSlot(0, InRowIndex)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("LaunchVariantLabel", "Variant"))
			];

		InValueColumn->AddSlot(0, InRowIndex)
			.HAlign(HAlign_Fill)
			[
				SNew(SProjectLauncherVariantSelector, DeviceProxy)
				.OnVariantSelected(this, &SProjectLauncherSimpleDeviceListRow::HandleVariantSelectorVariantSelected)
				.Text(this, &SProjectLauncherSimpleDeviceListRow::HandleVariantSelectorText)
			];
	}

	FString HandleVariantSelectorText() const
	{
		if (SimpleProfile.IsValid())
		{
			FName Variant = SimpleProfile->GetDeviceVariant();
			if (Variant == NAME_None)
			{
				return "Default";
			}
			return Variant.ToString();
		}

		return FString();
	}

	void HandleVariantSelectorVariantSelected(FName InVariant)
	{
		if (SimpleProfile.IsValid())
		{
			SimpleProfile->SetDeviceVariant(InVariant);
			UpdateProfile();
		}
	}

	// Build Config Settings Row

	void MakeBuildConfigurationRow(TSharedRef<SUniformGridPanel> InNameColumn, TSharedRef<SUniformGridPanel> InValueColumn, int InRowIndex)
	{
		InNameColumn->AddSlot(0, InRowIndex)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("LaunchConfigLabel", "Config"))
				.Visibility(this, &SProjectLauncherSimpleDeviceListRow::IsAdvancedVisible)
			];

		InValueColumn->AddSlot(0, InRowIndex)
			.HAlign(HAlign_Fill)
			[
				SNew(SProjectLauncherBuildConfigurationSelector)
				.OnConfigurationSelected(this, &SProjectLauncherSimpleDeviceListRow::HandleBuildConfigurationSelectorConfigurationSelected)
				.Text(this, &SProjectLauncherSimpleDeviceListRow::HandleBuildConfigurationSelectorText)
				.Visibility(this, &SProjectLauncherSimpleDeviceListRow::IsAdvancedVisible)
			];
	}

	FString HandleBuildConfigurationSelectorText() const
	{
		if (SimpleProfile.IsValid())
		{
			return EBuildConfigurations::ToString(SimpleProfile->GetBuildConfiguration());
		}

		return FString();
	}

	void HandleBuildConfigurationSelectorConfigurationSelected(EBuildConfigurations::Type Configuration)
	{
		if (SimpleProfile.IsValid())
		{
			SimpleProfile->SetBuildConfiguration(Configuration);
			UpdateProfile();
		}
	}

	// Cook Mode Settings Row

	void MakeCookModeRow(TSharedRef<SUniformGridPanel> InNameColumn, TSharedRef<SUniformGridPanel> InValueColumn, int InRowIndex)
	{
		InNameColumn->AddSlot(0, InRowIndex)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CookLabel", "Data Build"))
				.Visibility(this, &SProjectLauncherSimpleDeviceListRow::IsAdvancedVisible)
			];

		InValueColumn->AddSlot(0, InRowIndex)
			.HAlign(HAlign_Fill)
			[
				SNew(SProjectLauncherCookModeSelector)
				.OnCookModeSelected(this, &SProjectLauncherSimpleDeviceListRow::HandleCookModSelectorConfigurationSelected)
				.Text(this, &SProjectLauncherSimpleDeviceListRow::HandleCookModSelectorText)
				.Visibility(this, &SProjectLauncherSimpleDeviceListRow::IsAdvancedVisible)
			];
	}

	FString HandleCookModSelectorText() const
	{
		if (SimpleProfile.IsValid())
		{
			switch (SimpleProfile->GetCookMode())
			{
			case ELauncherProfileCookModes::DoNotCook:
				return TEXT("Do not cook");

			case ELauncherProfileCookModes::ByTheBook:
				return TEXT("By the book");

			case ELauncherProfileCookModes::OnTheFly:
				return TEXT("On the fly");
			}
		}

		return FString();
	}

	void HandleCookModSelectorConfigurationSelected(ELauncherProfileCookModes::Type CookMode)
	{
		if (SimpleProfile.IsValid())
		{
			SimpleProfile->SetCookMode(CookMode);
			UpdateProfile();
		}
	}

	void UpdateProfile()
	{
		LaunchProfile->SetDeploymentMode(ELauncherProfileDeploymentModes::FileServer);

		FName Variant = SimpleProfile->GetDeviceVariant();
		ILauncherDeviceGroupRef NewGroup = Model->GetProfileManager()->CreateUnmanagedDeviceGroup();
		NewGroup->AddDevice(DeviceProxy->GetTargetDeviceId(Variant));
		LaunchProfile->SetDeployedDeviceGroup(NewGroup);

		LaunchProfile->ClearCookedCultures();
		LaunchProfile->AddCookedPlatform(DeviceProxy->GetTargetPlatformName(Variant));

		bool Advanced = IsAdvanced.IsBound() && IsAdvanced.Get();
		if (Advanced)
		{
			LaunchProfile->SetBuildConfiguration(SimpleProfile->GetBuildConfiguration());
			LaunchProfile->SetCookMode(SimpleProfile->GetCookMode());
		}
	}

	ILauncherProfilePtr GetLaunchProfile() const
	{
		return LaunchProfile;
	}

	FReply OnLaunchClicked()
	{
		if (OnProfileRun.IsBound())
		{
			OnProfileRun.Execute(LaunchProfile.ToSharedRef());
		}

		return FReply::Handled();
	}

	// Callback for whether advanced options are shown.
	EVisibility IsAdvancedVisible() const
	{
		bool Advanced = IsAdvanced.IsBound() && IsAdvanced.Get();
		return Advanced ? EVisibility::Visible : EVisibility::Collapsed;
	}

	// Callback for getting the icon image of the device.
	const FSlateBrush* HandleDeviceImage( ) const
	{
		const PlatformInfo::FPlatformInfo* const PlatformInfo = PlatformInfo::FindPlatformInfo(*DeviceProxy->GetTargetPlatformName(SimpleProfile->GetDeviceVariant()));
		return (PlatformInfo) ? FEditorStyle::GetBrush(PlatformInfo->GetIconStyleName(PlatformInfo::EPlatformIconSize::Large)) : FStyleDefaults::GetNoBrush();
	}

	// Callback for getting the friendly name.
	FString HandleDeviceNameText() const
	{
		const FString& Name = DeviceProxy->GetName();

		if (Name.IsEmpty())
		{
			return LOCTEXT("UnnamedDeviceName", "<unnamed>").ToString();
		}

		return Name;
	}

	// Callback for getting the host name.
	FString HandleHostNameText( ) const
	{
		return DeviceProxy->GetHostName();
	}

	// Callback for getting the host user name.
	FString HandleHostUserText( ) const
	{
		return DeviceProxy->GetHostUser();
	}

	// Callback for getting the host platform name.
	FString HandleHostPlatformText( ) const
	{
		return DeviceProxy->GetTargetPlatformName(SimpleProfile->GetDeviceVariant());
	}

private:

	// Holds a pointer to the data model.
	FProjectLauncherModelPtr Model;

	// Holds a reference to the device proxy that is displayed in this row.
	ITargetDeviceProxyPtr DeviceProxy;

	// Holds a reference to the simple profile for this device
	ILauncherSimpleProfilePtr SimpleProfile;

	// Holds a reference to the generated full launch profile for this device.
	ILauncherProfilePtr LaunchProfile;

	// Specifies that whether options are shown.
	TAttribute<bool> IsAdvanced;

	// Holds a delegate to be invoked when a profile is run.
	FOnProfileRun OnProfileRun;
};


#undef LOCTEXT_NAMESPACE
