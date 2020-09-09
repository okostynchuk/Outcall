/**************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
**
** $QT_END_LICENSE$
**
**************************************************************************/

function Component()
{
    // constructor
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
}

Component.prototype.createOperations = function()
{
        component.createOperations();

        if (installer.value("os") === "win")
	{
	component.addOperation("CreateShortcut", 
                            "@TargetDir@/OutCALL.exe",// target
			    "@StartMenuDir@/OutCALL.lnk",
                            "workingDirectory=@TargetDir@",// working-dir
                            "iconPath=@TargetDir@/OutCALL.exe", "iconId=0",// icon
                            "description=Запустить OutCALL");// description
	component.addOperation("CreateShortcut",
				"@TargetDir@/OutCALL.exe",
				"@DesktopDir@/OutCALL.lnk",
				"workingDirectory=@TargetDir@",
				"iconPath=@TargetDir@/OutCALL.exe",
				"description=Start the app");

// Check if redistributable is installed
var registryVC2015x64 = installer.execute("reg", new Array("QUERY", "HKEY_CLASSES_ROOT\\Installer\\Dependencies\\VC,redist.x86,x86,14.26,bundle", "/v", "Version"))[0];
var registryVC2015x86 = installer.execute("reg", new Array("QUERY", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\Installer\\Dependencies\\Microsoft.VS.VC_RuntimeAdditionalVSU_x86,v14", "/v", "Version"))[0];

//Check if SQL server is installed
var registrySqlServerx64 = installer.execute("reg", new Array("QUERY", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft SQL Server Native Client 10.0\\CurrentVersion", "/v", "Version"))[0];
var registrySqlServerx86 = installer.execute("reg", new Array("QUERY", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SQL Server Native Client 10.0\\CurrentVersion", "/v", "Version"))[0];

	if (!registryVC2015x64 || !registryVC2015x86){
	component.addOperation("Execute", "{0,1602,1638,3010}", "@TargetDir@\\VC_redist.x86.exe", "/passive", "/norestart");
	}
	
	if (systemInfo.currentCpuArchitecture === "x86_64" || systemInfo.currentCpuArchitecture === "x64")
	{
		if(!registrySqlServerx64){
		component.addElevatedOperation("Execute", "{0,1602,1638,3010}", "@TargetDir@\\sqlncli_amd64.exe", "/passive", "/norestart");
		}
	}
	else 
	{
		if(!registrySqlServerx86){
		component.addElevatedOperation("Execute", "{0,1602,1638,3010}", "@TargetDir@\\sqlncli_x86.exe", "/passive", "/norestart");
		}
	}
	}
}