#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QThread>
#include <boost/shared_ptr.hpp>
#include <thread>
#include <string>
#include <vector>

// LSL API
#define LSL_DEBUG_BINDINGS
#include <lsl_cpp.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinIoCtl.h>

#include "LiveAmp.h"

namespace Ui {
class MainWindow;
}

class WaitThread : public QThread
{
	Q_OBJECT

protected:
	void run();
};



class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent, const std::string &config_file);
    ~MainWindow();
    

private slots:
    // config file dialog ops (from main menu)
    void load_config_dialog();
    void save_config_dialog();
	void versions_dialog();
	// get list of available devices
	void refresh_devices();

	// link to selected device
	void link();

    // close event (potentially disabled)
    void closeEvent(QCloseEvent *ev);

	void update_channel_labels_with_eeg(int);
	void update_channel_labels_with_bipolar(int);
	void update_channel_labels_aux(int);
	void update_channel_labels_acc(bool);
	
	// if the device combo box item changes
	void choose_device(int which);

	void radio_button_behavior(bool b);
	


private:

	int eegChannelCount;
	std::vector<int> usableChannelsByDevice;
	int bipolarChannelCount;
	bool overwrite;
	bool overrideAutoUpdate;
	bool check_configuration();
	void update_channel_counters(int n);
	void wait_message();
	void update_channel_labels(void);

	// background data reader thread
	void read_thread(int chunkSize, int samplingRate, std::vector<std::string> channelLabels);

	
	// container for amplifier enumeration
	//std::vector<std::pair<std::string, int>> ampData;

    // raw config file IO
    void load_config(const std::string &filename);
    void save_config(const std::string &filename);

	bool ConfigureLiveAmp(void);
	bool InitializeLiveAmp(void);
	
	void ExtractLiveAmpData(BYTE* buffer, int size, int samplesize, int *dataTypes, int usedChannelsCnt, std::vector<std::vector<double>> &extractData);
	bool EnableLiveAmpChannels(bool enableAUX, bool enableACC);
	bool GenerateUsedPhysicalChannelIndexes(void);
	void PrepareDataToSendOverLSL(std::vector<std::vector<double>> &LiveAmptData, std::vector<std::vector<double>> &LSLData, std::vector<uint16_t> &trigger_buffer);
	void AdjustChannelLabels(std::vector<std::string>& inpuChannelLabels, std::vector<std::string> &adjustedChannelLabels);
	
	bool unsampledMarkers;  
	bool sampledMarkers;   
	bool sampledMarkersEEG; 
	bool useSim;

	LiveAmp *liveAmp;
	std::vector<std::string> live_amp_sns;				// live amp serial number container

	std::unique_ptr<std::thread>  reader_thread_;	// our reader thread

    Ui::MainWindow *ui;

	bool use_simulators;
	bool stop_;											// whether the reader thread is supposed to stop

	t_TriggerOutputMode triggerOutputMode;
	struct t_AppVersion {
		int32_t Major;
		int32_t Minor;
	};
	t_AppVersion m_AppVersion;

};

#endif // MAINWINDOW_H