// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	SProjectLauncherCookPage.cpp: Implements the SProjectLauncherCookPage class.
=============================================================================*/

#include "ProjectLauncherPrivatePCH.h"


#define LOCTEXT_NAMESPACE "SProjectLauncherBuildPage"


/* SProjectLauncherCookPage structors
 *****************************************************************************/

SProjectLauncherBuildPage::~SProjectLauncherBuildPage( )
{
	if (Model.IsValid())
	{
		Model->OnProfileSelected().RemoveAll(this);
	}
}


/* SProjectLauncherCookPage interface
 *****************************************************************************/

void SProjectLauncherBuildPage::Construct( const FArguments& InArgs, const FProjectLauncherModelRef& InModel )
{
	Model = InModel;

	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
					.FillWidth(1.0)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
							.Text(LOCTEXT("BuildText", "Do you wish to build?"))
					]

				+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(8.0, 0.0, 0.0, 0.0)
					[
						// build mode check box
						SNew(SCheckBox)
						.IsChecked(this, &SProjectLauncherBuildPage::HandleBuildIsChecked)
						.OnCheckStateChanged(this, &SProjectLauncherBuildPage::HandleBuildCheckedStateChanged)
					]
			]

        + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 8.0f, 0.0f, 0.0f)
            [
                SNew(SExpandableArea)
                .AreaTitle(LOCTEXT("AdvancedAreaTitle", "Advanced Settings"))
                .InitiallyCollapsed(true)
                .Padding(8.0)
                .BodyContent()
                [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SButton)
                        .Text(LOCTEXT("GenDSYMText", "Generate DSYM"))
                        .IsEnabled( this, &SProjectLauncherBuildPage::HandleGenDSYMButtonEnabled )
                        .OnClicked( this, &SProjectLauncherBuildPage::HandleGenDSYMClicked )
                    ]
                ]
            ]

		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SProjectLauncherCookedPlatforms, InModel)
					.Visibility(this, &SProjectLauncherBuildPage::HandleBuildPlatformVisibility)
			]
	];

	Model->OnProfileSelected().AddSP(this, &SProjectLauncherBuildPage::HandleProfileManagerProfileSelected);
}


/* SProjectLauncherBuildPage implementation
 *****************************************************************************/

bool SProjectLauncherBuildPage::GenerateDSYMForProject( const FString& ProjectName, const FString& Configuration )
{
    // UAT executable
    FString ExecutablePath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() + FString(TEXT("Build")) / TEXT("BatchFiles"));
#if PLATFORM_MAC
    FString Executable = TEXT("RunUAT.command");
#elif PLATFORM_LINUX
    FString Executable = TEXT("RunUAT.sh");
#else
    FString Executable = TEXT("RunUAT.bat");
#endif

    // build UAT command line parameters
    FString CommandLine;
    CommandLine = FString::Printf(TEXT("GenerateDSYM -project=%s -config=%s"),
        *ProjectName,
        *Configuration);

    // launch the builder and monitor its process
    FProcHandle ProcessHandle = FPlatformProcess::CreateProc(*(ExecutablePath / Executable), *CommandLine, false, false, false, NULL, 0, *ExecutablePath, NULL);

	return ProcessHandle.Close();
}


/* SProjectLauncherBuildPage callbacks
 *****************************************************************************/


// Callback for changing the checked state of a platform menu check box. 
void SProjectLauncherBuildPage::HandleBuildCheckedStateChanged( ESlateCheckBoxState::Type CheckState )
{
	ILauncherProfilePtr SelectedProfile = Model->GetSelectedProfile();

	if (SelectedProfile.IsValid())
	{
		SelectedProfile->SetBuildGame(CheckState == ESlateCheckBoxState::Checked);
	}
}

// Callback for determining whether a platform menu entry is checked.
ESlateCheckBoxState::Type SProjectLauncherBuildPage::HandleBuildIsChecked() const
{
	ILauncherProfilePtr SelectedProfile = Model->GetSelectedProfile();

	if (SelectedProfile.IsValid())
	{
		if (SelectedProfile->IsBuilding())
		{
			return ESlateCheckBoxState::Checked;
		}
	}

	return ESlateCheckBoxState::Unchecked;
}

void SProjectLauncherBuildPage::HandleProfileManagerProfileSelected( const ILauncherProfilePtr& SelectedProfile, const ILauncherProfilePtr& PreviousProfile )
{
	// reload settings
}

EVisibility SProjectLauncherBuildPage::HandleBuildPlatformVisibility( ) const
{
	ILauncherProfilePtr SelectedProfile = Model->GetSelectedProfile();

	if (SelectedProfile.IsValid())
	{
		if (SelectedProfile->GetCookMode() == ELauncherProfileCookModes::DoNotCook && SelectedProfile->GetDeploymentMode() == ELauncherProfileDeploymentModes::DoNotDeploy)
		{
			return EVisibility::Visible;
		}
	}

	return EVisibility::Collapsed;
}

// Callback for pressing the Advanced Setting - Generate DSYM button.
FReply SProjectLauncherBuildPage::HandleGenDSYMClicked()
{
    ILauncherProfilePtr SelectedProfile = Model->GetSelectedProfile();

    if(SelectedProfile.IsValid())
    {
        if (!SelectedProfile->HasValidationError(ELauncherProfileValidationErrors::NoProjectSelected))
        {
            FString ProjectName = SelectedProfile->GetProjectName();
            EBuildConfigurations::Type ProjectConfig = SelectedProfile->GetBuildConfiguration();

            GenerateDSYMForProject( ProjectName, EBuildConfigurations::ToString(ProjectConfig) );
        }
    }

    return FReply::Handled();
}

bool SProjectLauncherBuildPage::HandleGenDSYMButtonEnabled() const
{
    ILauncherProfilePtr SelectedProfile = Model->GetSelectedProfile();

    if (SelectedProfile.IsValid())
    {
        if (!SelectedProfile->HasValidationError(ELauncherProfileValidationErrors::NoProjectSelected))
        {
            return true;
        }
    }

    return false;
}


#undef LOCTEXT_NAMESPACE
