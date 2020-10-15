function Controller()
{
}

Controller.prototype.WelcomePageCallback = function()
{
	if (installer.isInstaller() || installer.isUninstaller() || installer.isUpdater())
	{
		installer.autoRejectMessageBoxes();
	}
}

Controller.prototype.CredentialsPageCallback = function()
{
	if (installer.isInstaller() || installer.isUninstaller() || installer.isUpdater())
	{
		installer.autoRejectMessageBoxes();
	}
}

Controller.prototype.IntroductionPageCallback = function()
{
	if (installer.isInstaller() || installer.isUninstaller() || installer.isUpdater())
	{
		installer.autoRejectMessageBoxes();
	}
}

Controller.prototype.TargetDirectoryPageCallback = function()
{
	if (installer.isInstaller())
	{			
		widget.findChild("TargetDirectoryLineEdit").readOnly = true;
	}
}

Controller.prototype.StartMenuDirectoryPageCallback = function()
{
	if (installer.isInstaller() || installer.isUninstaller() || installer.isUpdater())
	{
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.ReadyForInstallationPageCallback = function()
{	
	if (installer.isInstaller())
	{  
		installer.autoRejectMessageBoxes();
	}
}

Controller.prototype.PerformInstallationPageCallback = function ()
{
	if (installer.isInstaller())
	{
		installer.autoRejectMessageBoxes();
	}
}