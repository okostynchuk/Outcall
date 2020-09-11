function Controller()
{
	installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
}

Controller.prototype.WelcomePageCallback = function()
{
	if (installer.isInstaller())
	{
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.CredentialsPageCallback = function()
{
	if (installer.isInstaller())
	{
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.IntroductionPageCallback = function()
{
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function()
{
	if (installer.isInstaller())
	{
		gui.clickButton(buttons.NextButton);	
		var widget = gui.currentPageWidget();
		widget.findChild("TargetDirectoryLineEdit").readOnly = true;
	}
}

Controller.prototype.ComponentSelectionPageCallback = function()
{
	if (installer.isInstaller())
	{
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.LicenseCheckPageCallback = function()
{
	if (installer.isInstaller())
	{
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.StartMenuDirectoryPageCallback = function()
{
	if (installer.isInstaller())
	{
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.ReadyForInstallationPageCallback = function()
{	
	if (installer.isInstaller())
	{  
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.PerformInstallationPageCallback = function ()
{
	if (installer.isInstaller())
	{
		installer.autoRejectMessageBoxes();
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.FinishedPageCallback = function ()
{
	if (installer.isInstaller())
	{
		gui.clickButton(buttons.FinishButton);
	}
}