// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once


/**
 * Implements the profile page for the session launcher wizard.
 */
class SProjectLauncherBuildPage
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SProjectLauncherBuildPage) { }
	SLATE_END_ARGS()

public:

	/**
	 * Destructor.
	 */
	~SProjectLauncherBuildPage( );

public:

	/**
	 * Constructs the widget.
	 *
	 * @param InArgs The Slate argument list.
	 * @param InModel The data model.
	 */
	void Construct(	const FArguments& InArgs, const FProjectLauncherModelRef& InModel );

protected:

	/**
	 * Generates the DSYM for the project.
	 *
	 * @param ProjectName The name of the project.
	 * @param Configuration The build configuration name.
	 * @return true on success, false otherwise.
	 */
    bool GenerateDSYMForProject( const FString& ProjectName, const FString& Configuration );

private:

	// Callback for changing the checked state of a platform menu check box. 
	void HandleBuildCheckedStateChanged( ESlateCheckBoxState::Type CheckState );

	// Callback for determining whether a platform menu entry is checked.
	ESlateCheckBoxState::Type HandleBuildIsChecked() const;

	// Callback for changing the selected profile in the profile manager.
	void HandleProfileManagerProfileSelected( const ILauncherProfilePtr& SelectedProfile, const ILauncherProfilePtr& PreviousProfile );

	// Callback for determining if the build platform list should be displayed
	EVisibility HandleBuildPlatformVisibility( ) const;

    // Callback for pressing the Advanced Setting - Generate DSYM button.
    FReply HandleGenDSYMClicked();

	// Callback for getting the enabled state of the Generate DSYM button.
    bool HandleGenDSYMButtonEnabled() const;

private:

	// Holds a pointer to the data model.
	FProjectLauncherModelPtr Model;
};
