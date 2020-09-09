function Controller()
{
	installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
}

Controller.prototype.WelcomePageCallback = function()
{
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.CredentialsPageCallback = function()
{
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.IntroductionPageCallback = function()
{
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function()
{
	gui.clickButton(buttons.NextButton);	
	var widget = gui.currentPageWidget();
	widget.findChild("TargetDirectoryLineEdit").readOnly = true;
}

Controller.prototype.ComponentSelectionPageCallback = function()
{
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseCheckPageCallback = function()
{
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.StartMenuDirectoryPageCallback = function()
{
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function()
{	
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.PerformInstallationPageCallback = function ()
{	
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function ()
{
	//gui.clickButton(buttons.FinishButton);
}