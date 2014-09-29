#include "tools/profiler/ui/profiler_window.h"

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "base/memory/scoped_vector.h"
#include "base/memory/weak_ptr.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "base/win/message_window.h"

#include "grit/app_resources.h"
#include "ui/base/models/combobox_model.h"
#include "ui/base/models/table_model.h"
#include "ui/base/ui_base_paths.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/win/hwnd_util.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/button/blue_button.h"
#include "ui/views/controls/combobox/combobox.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/table/table_view.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/window/custom_frame_view.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"

#include "tools/profiler/log_daemon.h"
#include "tools/profiler/profiler_thread.h"
#include "tools/profiler/profiler_data.h"
#include "tools/profiler/profiler_process.h"
#include "tools/profiler/profiler_cmds.h"

namespace views {
namespace profiler {

//////////////////////////////////////////////////////////////////////////

namespace {

ui::TableColumn BuildTableColumn(int id, const std::wstring& title,int width) {
  ui::TableColumn column;
  column.id = id;
  column.title = title;
  column.sortable = false;
	column.width = width;
  return column;
}
ui::TableColumn BuildTableColumn(int id, const std::wstring& title,float percent) {
  ui::TableColumn column;
  column.id = id;
  column.title = title;
  column.sortable = false;
	column.percent = percent;
  return column;
}

class LogTableModel : public ui::TableModel{
public:
  // ui::TableModel:
  virtual int RowCount() OVERRIDE{
		return dataset_.size();
	}
  virtual string16 GetText(int row, int column_id) OVERRIDE{
		return dataset_[row][column_id];
	}
  virtual void SetObserver(ui::TableModelObserver* observer) OVERRIDE{
		if(observer){
			observer_list_.AddObserver(observer);
		}else{
			observer_list_.Clear();
		}
	}
	// op
  void OnAction(const std::wstring& time_ms,const std::wstring& delta_ms,
			const std::wstring& pid,const std::wstring& tid,
			const std::wstring& action,const std::wstring& data){
		std::vector<std::wstring> dataitem;
		dataitem.push_back(time_ms);
		dataitem.push_back(delta_ms);
		dataitem.push_back(pid);
		dataitem.push_back(tid);
		dataitem.push_back(action);
		dataitem.push_back(data);
		dataset_.push_back(dataitem);
		FOR_EACH_OBSERVER(ui::TableModelObserver,observer_list_,OnItemsAdded(dataset_.size()-1,1));
	}
	void DeleteAllItems(){
		dataset_.clear();
		FOR_EACH_OBSERVER(ui::TableModelObserver,observer_list_,OnModelChanged());
	}

private:
  ObserverList<ui::TableModelObserver> observer_list_;
	std::vector<std::vector<std::wstring>> dataset_;
};

class ProcTableModel : public ui::TableModel{
public:
  // ui::TableModel:
  virtual int RowCount() OVERRIDE{
		return dataset_.size();
	}
  virtual string16 GetText(int row, int column_id) OVERRIDE{
		return dataset_[row][column_id];
	}
  virtual void SetObserver(ui::TableModelObserver* observer) OVERRIDE{
		if(observer){
			observer_list_.AddObserver(observer);
		}else{
			observer_list_.Clear();
		}
	}
	// op
	void OnProcData(const std::vector<ProfilerData::ProcData>& proc_data){
		dataset_.clear();
		for(size_t i=0;i<proc_data.size();i++){
			std::vector<std::wstring> dataitem;
			dataitem.push_back(proc_data[i].pid);
			dataitem.push_back(proc_data[i].private_memory);
			dataitem.push_back(proc_data[i].shared_memory);
			dataitem.push_back(proc_data[i].cpu);
			dataitem.push_back(proc_data[i].startup_ms);
			dataitem.push_back(proc_data[i].cmd_line);
			dataset_.push_back(dataitem);
		}
		FOR_EACH_OBSERVER(ui::TableModelObserver,observer_list_,OnModelChanged());
	}
	void DeleteAllItems(){
		dataset_.clear();
		FOR_EACH_OBSERVER(ui::TableModelObserver,observer_list_,OnModelChanged());
	}

private:
  ObserverList<ui::TableModelObserver> observer_list_;
	std::vector<std::vector<std::wstring>> dataset_;
};

class ProfilerButton : public views::LabelButton{
public:
  ProfilerButton(ButtonListener* listener, const string16& text):LabelButton(listener,text){
    SetStyle(Button::STYLE_BUTTON);
  }
  virtual ~ProfilerButton(){}
};

}  // namespace

//////////////////////////////////////////////////////////////////////////

class ProfilerWindowContents :	public WidgetDelegateView,
																public ButtonListener,
																public ProfilerData::Delegate{
 public:
  ProfilerWindowContents(Operation operation)
      : status_label_(new Label),
        operation_(operation){
    instance_ = this;
		log_server_.reset(new LogDaemon);
		log_server_->Init();

    bool result = window_.Create(
			base::Bind(&ProfilerWindowContents::ProcessLaunchNotification,base::Unretained(this)));
    CHECK(result && window_.hwnd());
  }
  virtual ~ProfilerWindowContents() {
		log_server_.reset();
		delete list_log_;
		delete list_proc_;
		timer_.Stop();
	}

  // Prints a message in the status area, at the bottom of the window.
  void SetStatus(const std::string& status) {
    status_label_->SetText(UTF8ToUTF16(status));
  }

	// base::MessageLoopForUI::Observer不能取到WM_COPYDATA=
	bool ProcessLaunchNotification(
						UINT message,
						WPARAM wparam,
						LPARAM lparam,
						LRESULT* result) {
		if (message != WM_COPYDATA)
			return false;
		COPYDATASTRUCT* cds = reinterpret_cast<COPYDATASTRUCT*>(lparam);

		struct cp_tag{
			int64 ticks_start;
			int64 ticks_end;
			int launch_ok;
		};

		if(cds->cbData!=sizeof(cp_tag) || !cds->lpData){
			return false;
		}

		cp_tag* cpdata = (cp_tag*)cds->lpData;
		ProfilerData* profiler_data = g_profiler_process->profiler_data();
		profiler_data->LaunchOk(cpdata->ticks_start,cpdata->ticks_end,!!cpdata->launch_ok);
    *result = TRUE;
		return true;
	}
  static ProfilerWindowContents* instance() { return instance_; }

 private:
  // Overridden from WidgetDelegateView:
  virtual bool CanResize() const OVERRIDE { return true; }
  virtual bool CanMaximize() const OVERRIDE { return true; }
  virtual string16 GetWindowTitle() const OVERRIDE {
    return ASCIIToUTF16("profiler");
  }
  virtual View* GetContentsView() OVERRIDE { return this; }
  virtual void WindowClosing() OVERRIDE {
    instance_ = NULL;
    if (operation_ == QUIT_ON_CLOSE)
      base::MessageLoopForUI::current()->Quit();
  }

	//custframeview太慢了,最好用native的,或者去掉GetWindowMask=todo(hege)
	virtual NonClientFrameView* CreateNonClientFrameView(Widget* widget) OVERRIDE{
#if 1
		widget->set_frame_type(Widget::FRAME_TYPE_FORCE_CUSTOM);
		CustomFrameView* custom_frame_view = new CustomFrameView;
		custom_frame_view->Init(widget);
		return custom_frame_view;
#else
		return NULL;
#endif
	}
	virtual gfx::ImageSkia GetWindowIcon() OVERRIDE{
		return *ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_PRODUCT_LOGO_16);
	}
	virtual bool ShouldShowWindowIcon() const OVERRIDE{
		return true;
	}

  // Overridden from View:
  virtual void ViewHierarchyChanged(
      const ViewHierarchyChangedDetails& details) OVERRIDE {
    if (details.is_add && details.child == this)
      InitProfilerWindow();
  }

	// Overriden from ButtonListener
	virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE{
		// G1
		if(sender == btn_export_){
		  ProfilerCmd::DumpGlobalVar(GetWidget()->GetNativeView());
		}
		if(sender == btn_dump_mini_){
		  ProfilerCmd::WriteMinidump(false);
		}
		if(sender == btn_dump_full_){
		  ProfilerCmd::WriteMinidump(true);
		}
		if(sender == btn_dump_open_){
		  ProfilerCmd::OpenMinidumpFolder();
		}
		if(sender == btn_parser_dump_){
			ProfilerCmd::ParserMinidump(GetWidget()->GetNativeView());
		}
		if(sender == btn_crash_){
			ProfilerCmd::CrashIt(list_proc_->FirstSelectedRow());
		}

		// G2
		if(sender == btn_runlow_se){
		  ProfilerCmd::RunLow(GetWidget()->GetNativeView(),L"360se6.exe",L"360se.exe");
		}
		if(sender == btn_limitedjob_se){
		  ProfilerCmd::LimitedJob(GetWidget()->GetNativeView(),L"360se6.exe",L"360se.exe");
		}
		if(sender == btn_debugrun_se){
			dataset_log_.DeleteAllItems();
			ProfilerCmd::DebugRun(L"360se6.exe",L"360se.exe");
		}
		if(sender == btn_runlow_js){
		  ProfilerCmd::RunLow(GetWidget()->GetNativeView(),L"360chrome.exe",L"360chrome.exe");
		}
		if(sender == btn_limitedjob_js){
		  ProfilerCmd::LimitedJob(GetWidget()->GetNativeView(),L"360chrome.exe",L"360chrome.exe");
		}
		if(sender == btn_debugrun_js){
			dataset_log_.DeleteAllItems();
			ProfilerCmd::DebugRun(L"360chrome.exe",L"360chrome.exe");
		}

		// G3
		if(sender->tag() >= 0x1000){
			dataset_log_.DeleteAllItems();
			GetProfilerData()->ProfileApp(sender->tag()-0x1000,window_.hwnd());
		}
	}

  // ProfilerData::Delegate
  virtual void OnAction(const std::wstring& time_ms,const std::wstring& delta_ms,
			const std::wstring& pid,const std::wstring& tid,
			const std::wstring& action,const std::wstring& data) OVERRIDE{
		dataset_log_.OnAction(time_ms,delta_ms,pid,tid,action,data);
	}
	virtual void OnProcData(const std::vector<ProfilerData::ProcData>& proc_data) OVERRIDE{
		dataset_proc_.OnProcData(proc_data);
	}

	// op
  void InitProfilerWindow() {
    set_background(Background::CreateStandardPanelBackground());
    GridLayout* layout = new GridLayout(this);
    SetLayoutManager(layout);

		// left center right
		ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddPaddingColumn(0, 4);
    column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                          GridLayout::USE_PREF, 0, 0);
    column_set->AddPaddingColumn(0, 4);
    
		// top cmds 1 2 3
    layout->StartRow(0.0f , 0);
    layout->AddView(GetButtonsG1());
    layout->StartRow(0.0f , 0);
    layout->AddView(GetButtonsG2());
    layout->StartRow(0.0f , 0);
    layout->AddView(GetButtonsG3());

		// log view
    layout->StartRow(0.6f , 0);
		layout->AddView(GetLogTable());
		layout->AddPaddingRow(0,4);

		// proc view
    layout->StartRow(0.4f , 0);
		layout->AddView(GetProcTable());

		// status view
    layout->StartRow(0.0f , 0);
		layout->AddView(status_label_);

		timer_.Start(FROM_HERE, base::TimeDelta::FromSeconds(1),this,&ProfilerWindowContents::OnTimer);
  }

	View* GetButtonsG1(){
		View* container = new View();
		container->SetLayoutManager(new BoxLayout(BoxLayout::kHorizontal,2,2,4));

		// group 1
		container->AddChildView(btn_dump_mini_ = new ProfilerButton(this,L"抓小Dump"));
		container->AddChildView(btn_dump_open_ = new ProfilerButton(this,L"打开Dump目录"));
		container->AddChildView(btn_crash_ = new ProfilerButton(this,L"崩溃吧"));

		container->AddChildView(btn_export_ = new ProfilerButton(this,L"导出全局变量"));
		container->AddChildView(btn_dump_full_ = new ProfilerButton(this,L"抓大Dump"));
		container->AddChildView(btn_parser_dump_ = new ProfilerButton(this,L"解析Dump"));

		// used by internal,disable them
		btn_export_->SetEnabled(false);
		btn_dump_full_->SetEnabled(false);
		btn_parser_dump_->SetEnabled(false);

		return container;
	}

	View* GetButtonsG2(){
		View* container = new View();
		container->SetLayoutManager(new BoxLayout(BoxLayout::kHorizontal,2,2,4));

		// group 2
		container->AddChildView(btn_runlow_se = new ProfilerButton(this,L"低权限运行se"));
		container->AddChildView(btn_limitedjob_se = new ProfilerButton(this,L"所限job运行se"));
		container->AddChildView(btn_debugrun_se = new ProfilerButton(this,L"调试运行se"));

		container->AddChildView(btn_runlow_js = new ProfilerButton(this,L"低权限运行js"));
		container->AddChildView(btn_limitedjob_js = new ProfilerButton(this,L"所限job运行js"));
		container->AddChildView(btn_debugrun_js = new ProfilerButton(this,L"调试运行js"));

		return container;
	}
	View* GetButtonsG3(){
		View* container = new View();
		container->SetLayoutManager(new BoxLayout(BoxLayout::kHorizontal,2,2,4));

		// group 3
		BuildAppButton(container);

		return container;
	}

	View* GetLogTable(){
		View* container = new View();
		container->SetLayoutManager(new FillLayout());

		std::vector<ui::TableColumn> columns;
		columns.push_back(BuildTableColumn(0, L"时间(毫秒)",80));
		columns.push_back(BuildTableColumn(1, L"耗时(毫秒)",80));
		columns.push_back(BuildTableColumn(2, L"进程",60));
		columns.push_back(BuildTableColumn(3, L"线程",60));
		columns.push_back(BuildTableColumn(4, L"动作",200));
		columns.push_back(BuildTableColumn(5, L"数据",1.0f));

		list_log_ = new TableView(&dataset_log_, columns, TEXT_ONLY, true);
		container->AddChildView(list_log_->CreateParentIfNecessary());

		return container;
	}

	View* GetProcTable(){
		View* container = new View();
		container->SetLayoutManager(new FillLayout());

		std::vector<ui::TableColumn> columns;
		columns.push_back(BuildTableColumn(0, L"进程",100));
		columns.push_back(BuildTableColumn(1, L"私有内存",100));
		columns.push_back(BuildTableColumn(2, L"公用内存",100));
		columns.push_back(BuildTableColumn(3, L"CPU",100));
		columns.push_back(BuildTableColumn(4, L"启动时间",100));
		columns.push_back(BuildTableColumn(5, L"命令行",1.0f));

		list_proc_ = new TableView(&dataset_proc_, columns, TEXT_ONLY, true);
		container->AddChildView(list_proc_->CreateParentIfNecessary());

		return container;
	}

	void BuildAppButton(View* container)
	{
		btn_app_.clear();
		GetProfilerData()->Init(this);
		int app_count = GetProfilerData()->GetAppCount();
		for(int i=0;i<app_count;i++){
			ProfilerData::AppData appdata;
			GetProfilerData()->GetAppData(i,appdata);
			ProfilerButton* btn = new ProfilerButton(this,appdata.app_name.c_str());
			btn->set_tag(0x1000+i);
			btn_app_.push_back(btn);
			container->AddChildView(btn);
		}
	}

	ProfilerData* GetProfilerData()
	{
		ProfilerData* profiler_data = g_profiler_process->profiler_data();
		DCHECK(profiler_data);
		return profiler_data;
	}

	void OnTimer(){
		GetProfilerData()->QueryProcData();
	}

private:
	static ProfilerWindowContents* instance_;
  Label* status_label_;
  const Operation operation_;

  ProfilerButton *btn_export_,*btn_dump_mini_,*btn_dump_full_,*btn_dump_open_,*btn_parser_dump_,*btn_crash_;
  ProfilerButton *btn_runlow_se,*btn_limitedjob_se,*btn_debugrun_se;
  ProfilerButton *btn_runlow_js,*btn_limitedjob_js,*btn_debugrun_js;
  std::vector<ProfilerButton*> btn_app_;
	
  TableView* list_log_;
  TableView* list_proc_;
	LogTableModel dataset_log_;
	ProcTableModel dataset_proc_;

	base::RepeatingTimer<ProfilerWindowContents> timer_;
	scoped_ptr<LogDaemon> log_server_;
	base::win::MessageWindow window_;  // The message-only window.

	DISALLOW_COPY_AND_ASSIGN(ProfilerWindowContents);
};

//////////////////////////////////////////////////////////////////////////

// static
ProfilerWindowContents* ProfilerWindowContents::instance_ = NULL;

void ShowProfilerWindow(Operation operation) {
  if (ProfilerWindowContents::instance()) {
    ProfilerWindowContents::instance()->GetWidget()->Activate();
  } else {
    Widget* window  = Widget::CreateWindowWithBounds(new ProfilerWindowContents(operation),gfx::Rect());
    gfx::CenterAndSizeWindow(NULL, window->GetNativeWindow(),gfx::Size(window->GetWindowBoundsInScreen().width(),768));
		window->Show();
  }
}

void LogStatus(const std::string& string) {
  ProfilerWindowContents::instance()->SetStatus(string);
}

}  // namespace profiler
}  // namespace views
