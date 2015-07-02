#include<iostream>
#include<sstream>
#include<cstdlib>
#include<string>
#include<vector>
#include<cassert>

using namespace std;

template<typename T>
string ToString( const T& obj )
{
	stringstream ss;
	ss << obj;
	return ss.str();
}

// Prefix common to all the command strings
static const string schtasksCommon = "schtasks /create /F /RL HIGHEST /SC ONCE ";

static const string USB_ENABLE_COMMAND = "REG ADD HKLM\\System\\CurrentControlSet\\Services\\UsbStor /f /v Start /t REG_DWORD /d 3";
static const string USB_DISABLE_COMMAND = "REG ADD HKLM\\System\\CurrentControlSet\\Services\\UsbStor /f /v Start /t REG_DWORD /d 4";

static const string CDROM_ENABLE_COMMAND = "REG ADD HKLM\\System\\CurrentControlSet\\Services\\cdrom /f /v Start /t REG_DWORD /d 1";
static const string CDROM_DISABLE_COMMAND = "REG ADD HKLM\\System\\CurrentControlSet\\Services\\cdrom /f /v Start /t REG_DWORD /d 4";

static const string AUDIO_ENABLE_COMMAND = "sc start audiosrv";
static const string AUDIO_DISABLE_COMMAND = "sc stop audiosrv";

static const string BLUETOOTH_ENABLE_COMMAND = "sc start bthserv";
static const string BLUETOOTH_DISABLE_COMMAND = "sc stop bthserv";

static const string INTERNET_ENABLE_COMMAND = "ipconfig /renew";
static const string INTERNET_DISABLE_COMMAND = "ipconfig /release";

static const vector<string> devices =
{
	"USB",
	"CDROM",
	"AUDIO",
	"BLUETOOTH",
	"INTERNET"
};

static const vector<string> enableCommands =
{
	USB_ENABLE_COMMAND,
	CDROM_ENABLE_COMMAND,
	AUDIO_ENABLE_COMMAND,
	BLUETOOTH_ENABLE_COMMAND,
	INTERNET_ENABLE_COMMAND
};

static const vector<string> disableCommands =
{
	USB_DISABLE_COMMAND,
	CDROM_DISABLE_COMMAND,
	AUDIO_DISABLE_COMMAND,
	BLUETOOTH_DISABLE_COMMAND,
	INTERNET_DISABLE_COMMAND
};

static string startTime, endTime, startDate, endDate;
static bool enable;

void ScheduleTask( const size_t& device )
{
	const string& enableCommand = enableCommands[device];
	const string& disableCommand = disableCommands[device];

	// The list of commands to execute finally
	vector<string> commands;

	// No scheduling
	if ( startTime.empty() )
	{
		if ( enable )
		{
			commands.push_back( enableCommand );
		}
		else
		{
			commands.push_back( disableCommand );
		}
	}
	// ScheduleTask
	else
	{
		// Enable at start time
		if ( enable )
		{
			string command;
			command += schtasksCommon + "/TN \"Enable " + devices[device] + "\" ";
			command += "/ST " + startTime;
			command += ( ( startDate.empty() ) ? "" : " /SD " + startDate ) + " ";
			command += "/TR \"" + enableCommand + "\"";

			commands.push_back( command );

			// Disable at end time
			if ( !endTime.empty() )
			{
				command = schtasksCommon + "/TN \"Disable " + devices[device] + "\" ";
				command += "/ST " + endTime;
				command += ( ( endDate.empty() ) ? "" : " /SD " + endDate ) + " ";
				command += "/TR \"" + disableCommand + "\"";

				commands.push_back( command );
			}
		}
		// Disable at start time
		else
		{
			string command;
			command += schtasksCommon + "/TN \"Disable " + devices[device] + "\" ";
			command += "/ST " + startTime;
			command += ( ( startDate.empty() ) ? "" : " /SD " + startDate ) + " ";
			command += "/TR \"" + disableCommand + "\"";

			commands.push_back( command );

			// Enable at end time
			if ( !endTime.empty() )
			{
				command = schtasksCommon + "/TN \"Enable " + devices[device] + "\" ";
				command += "/ST " + endTime;
				command += ( ( endDate.empty() ) ? "" : " /SD " + endDate ) + " ";
				command += "/TR \"" + enableCommand + "\"";

				commands.push_back( command );
			}
		}
	}

	for ( const auto& command : commands )
	{
		cout << command << endl;
		system( command.c_str() );
	}
}

int main()
{
	cout << "1. USB\n2. CD-ROM\n3. Audio\n4. Bluetooth\n5. Internet" << endl;

	string optionString;
	getline( cin, optionString );
	stringstream ss;
	ss << optionString;

	size_t option;
	vector<size_t> options;

	while ( ss >> option )
	{
		if ( option >= devices.size() )
		{
			cout << "Wrong option!!\n";
			return -1;
		}
		options.push_back( option - 1 );
	}

	cout << "\n1. Enable\n2. Disable\n";
	string in;
	getline( cin, in );
	assert( in.size() );
	enable = ( ( in[0] == '1' ) ? true : false );

	cout << "Enter start time ( HH:MM ) ( Leave blank for executing now )\n";
	getline( cin, startTime );

	if ( !startTime.empty() )
	{
		cout << "Enter end time ( HH:MM ) ( Leave blank for no end time )\n";
		getline( cin, endTime );

		cout << "Enter start date ( MM/DD/YY ) ( Leave blank for current date )\n";
		getline( cin, startDate );

		cout << "Enter end date ( MM/DD/YY ) ( Leave blank for current date )\n";
		getline( cin, endDate );

	}

	for ( const auto& it : options )
	{
		ScheduleTask( it );
	}

}

