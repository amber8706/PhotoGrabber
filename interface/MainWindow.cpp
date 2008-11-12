_create_index:1972: File or Directory already exists
Last message repeated 1 time
KERN: bfs: bfs_open_dir:1515: Invalid Argument
KERN: Last message repeated 2 times.
KERN: bfs: bfs_open_dir:1515: Invalid Argument
Last message repeated 2 times
KERN: USB Hub 1: port 0 disabled
KERN: bfs: bfs_open_dir:1515: Invalid Argument
Last message repeated 1 time
KERN: Last message repeated 2 times.
KERN: bfs: bfs_open_dir:1515: Invalid Argument
KERN: USB Hub 1: port 0 disabled
KERN: USB Hub 1: port 1 disabled
KERN: libnetwork.so running in R5 compatibility mode.
KERN: USB Hub 1: port 0 disabled
KERN: bfs: inode at 8071562464921518212 requested!
Last message repeated 10 times
KERN: bfs: bfs_open_dir:1515: Invalid Argument
KERN: Last message repeated 5 times.
KERN: bfs: inode at 8071562464921518212 requested!
KERN: bfs: bfs_open_dir:1515: Invalid Argument
KERN: bfs: inode at 8071562464921518212 requested!
KERN: bfs: bfs_open_dir:1515: Invalid Argument
KERN: bfs: inode at 8071562464921518212 requested!
Last message repeated 1 time
KERN: bfs: bfs_open_dir:1515: Invalid Argument
KERN: bfs: inode at 8071562464921518212 requested!
KERN: USB Hub 1: port 0 disabled
Last message repeated 2 times
KERN: usb_disk: device reports a lun count of 1
KERN: usb_disk: vendor_identification    "Nikon   "
KERN: usb_disk: product_identification   "Digital Camera  "
KERN: usb_disk: product_revision_level   "1.00"
KERN: bfs: _RemoveDuplicate:1486: No such file or directory
KERN: bfs: Could not find value in index "size"!
KERN: bfs: Remove:1696: No such file or directory
KERN: bfs: Could not find value in index "last_modified"!
                                                                                                                                                                                                                                                                                                                                                                                                                                               etViewColor(bg_color);
	// Create the Status Dock
	CreateStatusDock();
	//
	r.top=0;
	r.left=0;
	r.right-= B_V_SCROLL_BAR_WIDTH;
	r.bottom= becam_view->Bounds().bottom - 70;
	// Add the Grid View
	becam_gridview = new GridView(r,"gridview", B_FOLLOW_ALL, B_WILL_DRAW);	
	becam_scrollview = new BScrollView(
									"becam_scrollview",
									becam_gridview,
									B_FOLLOW_ALL_SIDES,
									B_WILL_DRAW | B_FRAME_EVENTS,
									false,
									true,
									B_PLAIN_BORDER
									);
    becam_view->AddChild(becam_scrollview);
    becam_gridview->TargetedByScrollView (becam_scrollview);	
	// add view to window
	AddChild(becam_view);
	//Set the focus to the listview
	becam_gridview->MakeFocus(true);
	#ifdef DEBUG
		lfmainw = fopen(INTF_LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Window created\n");
		fclose(lfmainw);
	#endif
}
//
// MainWindow:: Create the menubar
void BeCam_MainWindow::addMenuBar ()
{
	becam_menubar = new BMenuBar(Bounds(), "menu_bar");
	AddChild(becam_menubar);
	// Add File menu to the menu bar
	becam_fileMenu = new BMenu("File");
	becam_fileMenu->AddItem(new BMenuItem(_T("About") , new BMessage(ABOUT)));
	becam_fileMenu->AddSeparatorItem();
	becam_fileMenu->AddItem(new BMenuItem(_T("Preferences") , new BMessage(CONFIG),'P'));
	becam_fileMenu->AddSeparatorItem();
	becam_fileMenu->AddItem(new BMenuItem(_T("Quit"), new BMessage(QUIT), 'Q'));
	becam_menubar->AddItem(becam_fileMenu);
	//	Add extra BDCP menu
	BMenuItem *disconnect;
	becam_actionsMenu = new BMenu(_T("Actions"));
	becam_actionsMenu->AddItem(new BMenuItem(_T("Connect") , new BMessage(CAM_CONNECT)));
	becam_actionsMenu->AddItem(new BMenuItem(_T("Disconnect") , new BMessage(CAM_DISCON)));
	disconnect = becam_actionsMenu->FindItem(_T("Disconnect"));
	disconnect->SetEnabled(false);
	becam_menubar->AddItem(becam_actionsMenu);
	// Add the test menu
	#ifdef DEBUG	
		BMenu *debuggingMenu = new BMenu(_T("Debugging"));
		debuggingMenu->AddItem(new BMenuItem(_T("Open Statuswindow") , new BMessage(OPN_STATUS)));
		becam_menubar->AddItem(debuggingMenu);
	#endif	
	if(devicetype == TYPE_USB)
		becam_actionsMenu->SetEnabled(false);
}
//
// MainWindow:: Create the Action Bar with the action controls
void BeCam_MainWindow::CreateStatusDock ()
{
	
	BRect r = becam_view->Bounds();
	r.top = r.bottom - 70;
	becam_statusDock = new StatusDock(r,"statusdock", B_FOLLOW_BOTTOM | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_FRAME_EVENTS);
	becam_statusDock->SetStatusMessage("Please connect your camera.");
	becam_statusDock->ShowChildren(MODE_INIT);
	becam_view->AddChild(becam_statusDock);
}
//
// MainWindow:: Add item to the listview
void BeCam_MainWindow::addItem(class BeCam_Item *item)
{
	Lock();
	becam_gridview->AddItem(item);
	Unlock();
}
//
// MainWindow:: Add item to the listview
void BeCam_MainWindow::removeItem(class BeCam_Item *item)
{
	Lock();
	becam_gridview->RemoveItem(item);
	Unlock();
}
//
// MainWindow:: Clear the listview
void BeCam_MainWindow::clearItems()
{
	Lock();
	becam_gridview->MakeEmpty();
	Unlock();
}
//
// MainWindow:: Download the selected items
void BeCam_MainWindow::downloadSelectedItems(entry_ref copyToDir, const char *fileName = NULL)
{
	//
	if(becam_gridview->CurrentSelection() >= 0)
	{
		items_data *data = new items_data();
		data->statusDock = becam_statusDock;
		data->downloadDir = copyToDir;
        data->gridview = becam_gridview;
        data->window = this;
        data->fileName = fileName;
		resume_thread(spawn_thread((status_t(*)(void*))DownloadItems,"download_items",B_DISPLAY_PRIORITY,data));
	}
	else
	{
		BAlert *myAlert = new BAlert(_T("Download"), _T("Please select some files."),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
		myAlert->SetShortcut(0, B_ENTER);
		myAlert->Go();
	}
}
//
// MainWindow:: Download Items thread function
status_t BeCam_MainWindow::DownloadItems(items_data *data)
{
	entry_ref refentry;
	int count=0;
	int index=0;
	float totalSelectedItems = 0;
	char	tmpBuffer[100];
	BeCam_Item *selectedItem;
	BMessenger *messenger = NULL;
	BMessage *message = NULL;
	//
	refentry = data->downloadDir;
	//
	totalSelectedItems = data->gridview->GetNumberOfSelectedItems();
	sprintf(tmpBuffer,"Downloading number %ld of the %ld selected files",(uint32)1,(int32)totalSelectedItems);
	messenger = new BMessenger(data->window);
	message = new BMessage(STATDOCK_SHOW);
	message->AddFloat("numofselecitems",totalSelectedItems);
	message->AddString("statusmessage",tmpBuffer);
	messenger->SendMessage(message);
	delete(message);
	delete(messenger);
	while((index = data->gridview->CurrentSelection(index)) >= 0)
	{
		// Update the status dock
		sprintf(tmpBuffer,"Downloading number %ld of the %ld selected files",(uint32)count + 1,(int32)totalSelectedItems);
		messenger = new BMessenger(data->statusDock);
		message = new BMessage(UPDATE_STAT);
		message->AddFloat("count",1);
		message->AddString("statusmessage",tmpBuffer);
		messenger->SendMessage(message);
		delete(messenger);
		// Get selected item
		selectedItem = (BeCam_Item *)data->gridview->ItemAt(index);
		// Send a message to the camera interface to get the selected item
		message = new BMessage(DOWN_ITEM);
		message->AddInt32("itemhandle",(int32)selectedItem->GetHandle());
		if(data->gridview->GetNumberOfSelectedItems() == 1)
			message->AddString("name",data->fileName);
		#ifdef DEBUG
			BPath directory;
			directory = BPath(&data->downloadDir);
			lfmainw = fopen(INTF_LOGFILE,"a");	
			fprintf(lfmainw,"MAINWINDOW - The save directory is: %s\n",directory.Path());
			fclose(lfmainw);
		#endif
		message->AddRef("copyToDir", &refentry);
		// Wait untill the item has been downloaded
		messenger =  new BMessenger(NULL,data->window->systemcore);
		BMessage reply;
		messenger->SendMessage(message,&reply);
		if(reply.what == DOWN_ITEM_OK)
			count++;
		else
		{
			BAlert *myAlert = new BAlert(_T("Error"), _T("Failed to copy an item."),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
			myAlert->SetShortcut(0, B_ENTER);
			myAlert->Go();
			break;
		}
		delete(messenger);
		delete(message);
		index++;
	}
	messenger = new BMessenger(data->window);
	message = new BMessage(STATDOCK_HIDE);
	messenger->SendMessage(message);
	delete(message);
	delete(messenger);
}
//
// MainWindow:: Remove the selected items
void BeCam_MainWindow::removeSelectedItems()
{
	//
	#ifdef DEBUG
		lfmainw = fopen(INTF_LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Begin remove Items\n");
		fclose(lfmainw);
	#endif
	if(becam_gridview->CurrentSelection() >= 0)
	{
		// Ask the user if he/she is sure to remove the files.
		BAlert *myAlert = new BAlert(_T("Remove files"), _T("Are you sure you want to erase the selected files?"),_T("No"), _T("Yes"),NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
		myAlert->SetShortcut(0, B_ESCAPE);
		uint32 button_index = myAlert->Go();
		if(button_index == 1)
		{
			items_data *data = new items_data();
        	data->gridview = becam_gridview;
        	data->window = this;
			resume_thread(spawn_thread((status_t(*)(void*))RemoveItems,"remove_items",B_DISPLAY_PRIORITY,data));
		}
	}
	else
	{	
		BAlert *myAlert = new BAlert(_T("Remove"), _T("Please select some files."),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);	
		myAlert->SetShortcut(0, B_ENTER);
		myAlert->Go();
	}
	#ifdef DEBUG
		lfmainw = fopen(INTF_LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - End remove Items\n");
		fclose(lfmainw);
	#endif
			
}
//
//	MainWindow:: Remove Items thread function
status_t BeCam_MainWindow::RemoveItems(items_data *data)
{
	int count=0,index=0;
	BMessage *cam_message;
		
	count = data->gridview->CountItems();
	BeCam_Item *selectedItem;
	while((index = data->gridview->CurrentSelection(index)) >= 0)
	{
		selectedItem = (BeCam_Item *)data->gridview->ItemAt(index);
		// Send a message to the camera 
		// to get the selected item
		cam_message = new BMessage(REM_ITEM);
		cam_message->AddInt32("itemhandle", (int32)selectedItem->GetHandle());
		// Wait untill the item has been downloaded
		BMessenger messenger(NULL,data->window->systemcore);
		BMessage reply;
		messenger.SendMessage(cam_message,&reply);
		if(reply.what == REM_ITEM_OK)
		{
			data->window->removeItem(selectedItem);
			selectedItem = NULL;
			index = 0;
			delete(cam_message);
			continue;
		}
		delete(cam_message);
		index++;
	}
	return B_NO_ERROR;
}
//
//	MainWindow:: Error messages
int BeCam_MainWindow::logMainWindowError(int ErrorMes)
{
	char 				*errorMessage;
	
	switch(ErrorMes)
	{
		case MAINW_DEV_NO_FIND:
			errorMessage = "MAIN WINDOW: No PTP device is present\n";	
			break;
		default:
			errorMessage = "MAIN WINDOW:An unexpected error occured\n";
	}
	// write the errorMessage on the screen
	if(DEBUG_SCREEN)
		printf(errorMessage);
	// write the errorMessage into the logfile
	if(DEBUG_LOGFILE)
	{
		FILE	*file;
		
		file = fopen(INTF_LOGFILE,"a");
		fprintf(file,errorMessage);
		fclose(file);
	}
	return(ErrorMes);
}

//		BeCam_MainWin::QuitRequested
//

bool BeCam_MainWindow::QuitRequested()
{
	systemcore->PostMessage(CAM_DISCON);
	systemcore->PostMessage(B_QUIT_REQUESTED);
	return BWindow::QuitRequested();
}
//
//	BeCam_MainWindow:: Open the Configuration Window
void BeCam_MainWindow::CreateConfigWindow()
{
	float xPos=0,yPos=0;
	
	if(!configWindow)
	{
		CalculatePos(&xPos,&yPos,CONFIG_WINDOW);
		configWindow = new BeCam_ConfigWindow(xPos,yPos,this);
		configWindow->Show();
	}
}

//
//	BeCam_MainWindow:: Open the About Window
void BeCam_MainWindow::CreateAboutWindow()
{
	float xPos=0,yPos=0;	
	
	if(!aboutWindow)
	{
		CalculatePos(&xPos,&yPos,ABOUT_WINDOW);
		aboutWindow = new BeCam_AboutWindow(xPos,yPos,this);
	}
	aboutWindow->Show();
}
//
//	BeCam_MainWindow:: Open the Status Window
void BeCam_MainWindow::ShowStatusDock(float totalbytes, const char *message)
{
	becam_statusDock->ShowChildren(MODE_DOWNLOAD);
	becam_statusDock->SetMaxStatusBar(totalbytes);
	becam_statusDock->UpdateStatus(0,message);
	becam_statusDock->Show();
}
//
//		BeCam_MainWindow::Update the Status Window
void BeCam_MainWindow::UpdateStatusDock(uint32 delta, const char *message)
{
	//nothing yet
}
//
//	BeCam_MainWindow:: Close the Status Window
void BeCam_MainWindow::HideStatusDock(void)
{
	becam_statusDock->Hide();
	becam_statusDock->ShowChildren(MODE_INIT);
}
//
//	BeCam_MainWindow:: Calculate the windowposition
void BeCam_MainWindow::CalculatePos(float *xPos,float *yPos,int winType)
{
	float length,height;
	BRect mainRect = Frame();
	length = mainRect.right - mainRect.left;
	height = mainRect.bottom - mainRect.top;
	switch(winType)
	{
		case ABOUT_WINDOW:
			*xPos = ((mainRect.left + length/2) -(WINDOW_WIDTH_ABOUT/2));
			*yPos = ((mainRect.top + height/2) - (WINDOW_HEIGHT_ABOUT/2));
			break;
		case CONFIG_WINDOW:
			*xPos = ((mainRect.left + length/2) -(WINDOW_WIDTH_CONFIG/2));
			*yPos = ((mainRect.top + height/2) - (WINDOW_HEIGHT_CONFIG/2));
			break;
	}
}
//
//		BeCam_MainWin::MessageRecieved
void BeCam_MainWindow::MessageReceived(BMessage* message)
{
	const char *product;
			
	switch(message->what)
		{
		
		case QUIT:
			systemcore->PostMessage(CAM_DISCON);
			systemcore->PostMessage(B_QUIT_REQUESTED);
			break;
		case CAM_CONNECTED:
		{
			product = message->FindString("product");
			becam_connected = true;
			becam_statusDock->ShowChildren(MODE_CONNECTED);
			BMessenger messenger(becam_statusDock);
			BMessage *statDockmessage = new BMessage(UPDATE_STAT);
			statDockmessage->AddFloat("count",-1);
			statDockmessage->AddString("statusmessage",_T("Processing images..."));
			messenger.SendMessage(statDockmessage);
			break;
		}
		case GET_ITEMS_DONE:
			becam_statusDock->Hide();
			break;
		case CAM_DISCONNECTED:
			product = message->FindString("product");
			becam_connected = false;	
			clearItems();
			becam_statusDock->ShowChildren(MODE_INIT);
			becam_statusDock->SetStatusMessage(_T("Camera disconnected"));
			becam_statusDock->Show();
			break;
		case ADD_ITEM:
		{
			ItemData  *localItemData;
			BeCam_Item *localItem;
			message->FindPointer("item", (void **) &localItemData);
			localItem = new BeCam_Item(localItemData);
			addItem(localItem);
			break;
		}
		case CAM_CONNECT:
		{
			BMenuItem *connect,*disconnect;
			BMessenger messenger(NULL,systemcore);
			BMessage reply;
			BMessage cam_message(CAM_CONNECT);
			messenger.SendMessage(&cam_message,&reply);
			if(reply.what == CAM_CONNECT_OK)
			{
				systemcore->PostMessage(message);
				connect = becam_actionsMenu->FindItem(_T("Connect"));	
				connect->SetEnabled(false);
				disconnect = becam_actionsMenu->FindItem(_T("Disconnect"));	
				disconnect->SetEnabled(true);
				//
				becam_statusDock->SetStatusMessage(_T("Connecting to camera..."));
			}
			else
			{	
				BAlert *myAlert = new BAlert(_T("Connect"), _T("Could not connect to the camera"),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
				myAlert->SetShortcut(0, B_ENTER);
				myAlert->Go();
			}	
			break;
		}
		case CAM_DISCON:
		{	
			BMenuItem *connect,*disconnect;
			systemcore->PostMessage(message);	
			connect = becam_actionsMenu->FindItem(_T("Connect"));
			connect->SetEnabled(true);	
			disconnect = becam_actionsMenu->FindItem(_T("Disconnect"));
			disconnect->SetEnabled(false);
			break;
		}
		case B_COPY_TARGET:
		{
			entry_ref copyToDirDrag;
			const char *fileName = NULL;
			message->FindRef("directory", &copyToDirDrag);
			fileName = message->FindString("name");
			#ifdef DEBUG
				BPath directory;
				directory = BPath(&copyToDirDrag);
				lfmainw = fopen(INTF_LOGFILE,"a");	
				fprintf(lfmainw,"MAINWINDOW - The file name is: %s\n",fileName);
				fprintf(lfmainw,"MAINWINDOW - The save directory is: %s\n",directory.Path());
				fclose(lfmainw);
			#endif
			downloadSelectedItems(copyToDirDrag, fileName);
			break;
		}
		case REM_ITEMS:
		{
			removeSelectedItems();
			break;
		}
		case OPN_STATUS:
		{	
			char	tmpBuffer[100];
			sprintf(tmpBuffer,"Downloading number %ld of the %ld selected files",(uint32)0,(uint32)0);
			ShowStatusDock(0, tmpBuffer);
			break;
		}
		case ABOUT:
			CreateAboutWindow();
			break;
		case CONFIG:
			CreateConfigWindow();
			break;
		case B_SELECT_ALL:
			becam_gridview->SelectAll();
			break;
		case GET_CONFIGURATION:
		{
             // Get the settings from the system core
			BMessage reply;
			BMessage message(GET_CONFIGURATION);
			BMessenger messenger(NULL,systemcore);
			messenger.SendMessage(&message,&reply);
			reply.FindPointer("settings",(void **)&pgsettings);
			BMenuItem *morePath;
            break;
		}
		case RELOAD_CONFIGURATION:
		{
			//	Add extra BDCP menu
			BMessage reply;
			int32 devtype;
			BMessage message(GET_DEVTYPE);
			BMessenger messenger(NULL,systemcore);
			messenger.SendMessage(&message,&reply);
			reply.FindInt32("devtype",&devtype);
			#ifdef DEBUG
				lfmainw = fopen(INTF_LOGFILE,"a");	
				fprintf(lfmainw,"MAINWINDOW - device type is: %ld\n",devtype);
				fclose(lfmainw);
			#endif
			if(devtype == TYPE_PAR)
			{
				becam_actionsMenu->SetEnabled(true);
			}
			else
			{
				becam_actionsMenu->SetEnabled(false);
			}
			break;
		}
		case STATDOCK_SHOW:
		{
			const char *statusmsg;
			float numSelectedItems;
			statusmsg = message->FindString("statusmessage");
			message->FindFloat("numofselecitems",&numSelectedItems);
			ShowStatusDock((int32)numSelectedItems,statusmsg);
			break;
		}
		case STATDOCK_HIDE:
		{
			HideStatusDock();
			break;
		}
		case STATDOCK_SHOWED:
		{
			becam_gridview->SetEnabled(false);
			becam_scrollview->ResizeTo(becam_scrollview->Frame().Width(),becam_scrollview->Frame().Height() - becam_statusDock->Frame().Height());
			BScrollBar *v_scrollbar = becam_scrollview->ScrollBar(B_VERTICAL);
			v_scrollbar->ResizeTo(v_scrollbar->Frame().Width(),v_scrollbar->Frame().Height() + B_H_SCROLL_BAR_HEIGHT);
			break;
		}
		case STATDOCK_HIDED:
		{
			becam_gridview->SetEnabled(true);
			becam_scrollview->ResizeTo(becam_scrollview->Frame().Width(),becam_scrollview->Frame().Height() + becam_statusDock->Frame().Height());
			BScrollBar *v_scrollbar = becam_scrollview->ScrollBar(B_VERTICAL);
			v_scrollbar->ResizeTo(v_scrollbar->Frame().Width(),v_scrollbar->Frame().Height() - B_H_SCROLL_BAR_HEIGHT);
			break;
		}
		default:
			BWindow::MessageReceived(message);
		}
}

