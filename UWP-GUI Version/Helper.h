#pragma once
#include "pch.h"
#include<string>
using namespace Platform;
using namespace std;
class Methods {
public:
	static void MessageBoxInvoke(String^ title, String^ content, String^ btn1) {
		Windows::UI::Popups::MessageDialog^ Message = ref new Windows::UI::Popups::MessageDialog(content);
		Message->Title = title;
		auto btn = ref new Windows::UI::Popups::UICommand("OK");
		btn->Id = 0;
		Message->Commands->Append(btn);
		Message->CancelCommandIndex = 0;
		concurrency::create_task(Message->ShowAsync());
	}
	static string ToCppString(String^ s) {
		string result;
		for (auto i : s) {
			result += i == L'\r' ? '\n' : i;
		}
		return result;
	}
	static String^ FromCppString(std::string s) {
		;
		std::wstring ws;
		ws.assign(s.begin(), s.end());
		String^ result = ref new String(ws.c_str());
		return result;
	}
	static int StringToInt(String^ s) {
		string new_s = ToCppString(s).c_str();
		int n = 0;
		for (int i = 0; i < new_s.size(); i++) {
			n = n * 10 + new_s[i] - '0';
		}
		return n;
	}
};
