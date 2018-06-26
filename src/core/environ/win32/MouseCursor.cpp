﻿
#include "tjsCommHead.h"

#include "MouseCursor.h"
#include "WindowFormUnit.h"

const LPTSTR MouseCursor::CURSORS[CURSOR_EOT] = {
	IDC_APPSTARTING,
	IDC_ARROW,
	IDC_CROSS,
	IDC_HAND,
	IDC_IBEAM,
	IDC_HELP,
	IDC_NO,
	IDC_SIZEALL,
	IDC_SIZENESW,
	IDC_SIZENS,
	IDC_SIZENWSE,
	IDC_SIZEWE,
	IDC_UPARROW,
	IDC_WAIT,
};

const int MouseCursor::CURSOR_INDEXES[MouseCursor::CURSOR_INDEXES_NUM] = {
	CURSOR_SIZEALL, // crSizeAll = -22,
	CURSOR_HAND, // crHandPoint = -21,
	CURSOR_HELP, // crHelp = -20,
	CURSOR_ARROW, // crAppStart = -19,
	CURSOR_NO, // crNo = -18,
	CURSOR_ARROW, // crSQLWait = -17,
	CURSOR_ARROW, // crMultiDrag = -16,
	CURSOR_ARROW, // crVSplit = -15,
	CURSOR_ARROW, // crHSplit = -14,
	CURSOR_ARROW, // crNoDrop = -13,
	CURSOR_ARROW, // crDrag = -12,
	CURSOR_WAIT, // crHourGlass = -11,
	CURSOR_UPARROW, // crUpArrow = -10,
	CURSOR_SIZEWE, // crSizeWE = -9,
	CURSOR_SIZENWSE, // crSizeNWSE = -8,
	CURSOR_SIZENS, // crSizeNS = -7,
	CURSOR_SIZENESW, // crSizeNESW = -6,
	CURSOR_SIZEALL, // crSize = -5,
	CURSOR_IBEAM, // crIBeam = -4,
	CURSOR_CROSS, // crCross = -3,
	CURSOR_ARROW, // crArrow = -2,
	-1, // crNone = -1,
	CURSOR_ARROW, // crDefault = 0,
	CURSOR_ARROW, // crHBeam = 1,
};
std::vector<HCURSOR> MouseCursor::CURSOR_HANDLES_FOR_INDEXES;
HCURSOR MouseCursor::CURSOR_HANDLES[CURSOR_EOT];
bool MouseCursor::CURSOR_INITIALIZED = false;
bool MouseCursor::is_cursor_hide_ = false;

void MouseCursor::Initialize() {
	if( CURSOR_INITIALIZED ) return;

	// デフォルトカーソルを読み込む
	for( int i = 0; i < CURSOR_EOT; i++ ) {
		CURSOR_HANDLES[i] = ::LoadCursor( NULL, CURSORS[i] );
	}

	// 公開カーソルIDと一致するようにハンドルを格納する
	CURSOR_HANDLES_FOR_INDEXES.clear();
	CURSOR_HANDLES_FOR_INDEXES.reserve( CURSOR_INDEXES_NUM );
	for( int i = 0; i < CURSOR_INDEXES_NUM; i++ ) {
		int index = CURSOR_INDEXES[i];
		if( index >= 0 ) {
			CURSOR_HANDLES_FOR_INDEXES.push_back( CURSOR_HANDLES[index] );
		} else {
			CURSOR_HANDLES_FOR_INDEXES.push_back( INVALID_HANDLE_VALUE );
		}
	}
	CURSOR_INITIALIZED = true;
}
void MouseCursor::Finalize() {
	// 読み込まれたカーソルを削除
	if( CURSOR_HANDLES_FOR_INDEXES.size() > CURSOR_INDEXES_NUM ) {
		int numCursor = CURSOR_HANDLES_FOR_INDEXES.size();
		for( int i = CURSOR_INDEXES_NUM; i < numCursor; i++ ) {
			::DestroyCursor( CURSOR_HANDLES_FOR_INDEXES[i] );
			CURSOR_HANDLES_FOR_INDEXES[i] = INVALID_HANDLE_VALUE;
		}
	}
}
void MouseCursor::UpdateCurrentCursor() {
	int oldindex = cursor_index_;
	cursor_index_ = INVALID_CURSOR_INDEX;
	HCURSOR hCursor = ::GetCursor();
	int size = CURSOR_HANDLES_FOR_INDEXES.size();
	int handleIndex = oldindex+CURSOR_OFFSET;

	if( handleIndex >= 0 && handleIndex < size && CURSOR_HANDLES_FOR_INDEXES[handleIndex] == hCursor ) {
		// まずは旧カーソルと比較
		cursor_index_ = oldindex; // 変更されていない
	} else if( CURSOR_HANDLES[CURSOR_ARROW] == hCursor ) {
		// 次にデフォルトと比較
		cursor_index_ = crDefault;
	} else {
		// 以前と異なり、デフォルトでもない時は、全てと比較する
		for( int i = 0; i < size; i++ ) {
			if( CURSOR_HANDLES_FOR_INDEXES[i] == hCursor ) {
				cursor_index_ = i - CURSOR_OFFSET;
				break;
			}
		}
	}
}
void MouseCursor::SetMouseCursor( int index ) {
	if( is_cursor_hide_ && index != crNone ) {
		::ShowCursor( TRUE );
		is_cursor_hide_ = false;
	}
	if( index == crNone ) {
		if( is_cursor_hide_ != true ) {
			::ShowCursor( FALSE );
			is_cursor_hide_ = true;
		}
	} else {
		int id = index + CURSOR_OFFSET; // crSizeAll = -22, なので、その分加算
		if( id >= 0 && id < (int)CURSOR_HANDLES_FOR_INDEXES.size() ) {
			::SetCursor( CURSOR_HANDLES_FOR_INDEXES[id] );
		} else {
			::SetCursor( CURSOR_HANDLES[CURSOR_ARROW] );
		}
	}
}
void MouseCursor::SetCursor() {
	if( cursor_index_ != INVALID_CURSOR_INDEX ) {
		SetMouseCursor( cursor_index_ );
	}
}

void MouseCursor::SetCursorIndex( int index ) {
	cursor_index_ = index;
}
