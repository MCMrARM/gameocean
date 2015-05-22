package net.lbsg.client;

import android.app.NativeActivity;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.LinearLayout.LayoutParams;
import android.widget.PopupWindow;

public class MainActivity extends NativeActivity {

	boolean showKeyboard = false;
	String currentKeyboardText = null;
	PopupWindow window;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	}

	@Override
	protected void onPause() {
		super.onPause();

		if (window != null) {
			if (window.getContentView() instanceof EditText) {
				currentKeyboardText = ((EditText) window.getContentView())
						.getText().toString();

				InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
				imm.hideSoftInputFromWindow(window.getContentView()
						.getWindowToken(), 0);

				window.dismiss();
				window = null;
			}
		}
	}

	@Override
	protected void onResume() {
		super.onResume();

		if (showKeyboard) {
			showKeyboard(currentKeyboardText);
		}
	}

	public void showKeyboard(final String currentText) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (window != null)
					return;
				showKeyboard = true;

				final CustomEditText editText = new CustomEditText(MainActivity.this);
				
				editText.addTextChangedListener(new TextWatcher() {
					
					@Override
					public void onTextChanged(CharSequence s, int start, int before, int count) {
						onTextUpdated(editText.getText().toString());
					}
					
					@Override
					public void beforeTextChanged(CharSequence s, int start, int count,
							int after) {
						// 
					}
					
					@Override
					public void afterTextChanged(Editable s) {
						// 
					}
				});

				editText.setText(currentText);
				editText.setSingleLine(true);
				editText.setImeOptions(EditorInfo.IME_FLAG_NO_FULLSCREEN
						| EditorInfo.IME_FLAG_NO_EXTRACT_UI);
				editText.setWidth(0);
				editText.setHeight(0);

				window = new PopupWindow(editText, LayoutParams.MATCH_PARENT,
						LayoutParams.MATCH_PARENT);
				window.setWindowLayoutMode(LayoutParams.WRAP_CONTENT,
						LayoutParams.WRAP_CONTENT);
				window.setInputMethodMode(PopupWindow.INPUT_METHOD_NEEDED);
				window.setTouchable(false);
				window.setFocusable(true);
				window.setBackgroundDrawable(new ColorDrawable());
				window.showAtLocation(getWindow().getDecorView(),
						Gravity.CENTER, 0, 0);

				editText.requestFocus();
				editText.setSelection(editText.getText().length());

				InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
				imm.showSoftInput(getWindow().getDecorView(),
						InputMethodManager.SHOW_FORCED);
			}
		});
	}

	public void hideKeyboard() {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (!showKeyboard)
					return;
				showKeyboard = false;

				if (window == null)
					return;

				if (window.getContentView() instanceof EditText) {
					InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
					imm.hideSoftInputFromWindow(window.getContentView()
							.getWindowToken(), 0);

					window.dismiss();
					window = null;
				}
			}
		});
	}
	
	public static native void onTextUpdated(String newText);

	public static class CustomEditText extends EditText {

		MainActivity activity;

		public CustomEditText(MainActivity activity) {
			super(activity);
			this.activity = activity;
		}

		@Override
		public boolean onKeyPreIme(int keyCode, KeyEvent event) {
			if (keyCode == KeyEvent.KEYCODE_BACK) {
				this.activity.hideKeyboard();

				return true;
			}

			return true;
		}
		
		@Override
		public void onEditorAction(int actionCode) {
			if ((actionCode & EditorInfo.IME_MASK_ACTION) == EditorInfo.IME_ACTION_DONE) {
				this.activity.hideKeyboard();
			}
			super.onEditorAction(actionCode);
		}
		
		@Override
		public void draw(Canvas canvas) {
			//
		}

	}
	
	static {
		System.loadLibrary("client");
	}

}
