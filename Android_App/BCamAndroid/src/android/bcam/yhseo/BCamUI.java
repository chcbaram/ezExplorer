package android.bcam.yhseo;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.RejectedExecutionHandler;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.ReentrantLock;
import java.util.zip.Inflater;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.*;
import android.widget.*;
import android.graphics.*;
import android.graphics.drawable.Drawable;

public class BCamUI extends Activity implements RejectedExecutionHandler {

	private final class ConnectedTask implements Cancelable {

		private final AtomicBoolean mmClosed = new AtomicBoolean();
		private final InputStream mmInStream;
		private final OutputStream mmOutStream;
		private final BluetoothSocket mmSocket;

		public ConnectedTask(BluetoothSocket socket) {
			InputStream in = null;
			OutputStream out = null;
			try {
				in = socket.getInputStream();
				out = socket.getOutputStream();
			} catch (IOException e) {
				Log.e(Constants.TAG, "sockets not created", e);
			}
			mmSocket = socket;
			mmInStream = in;
			mmOutStream = out;
		}

		public void cancel() {
			if (mmClosed.getAndSet(true)) {
				return;
			}
			try {
				mmSocket.close();
			} catch (IOException e) {
				Log.e(Constants.TAG, "close failed", e);
			}
		}
		
		public void sendOneByte(byte b){
			if (mmOutStream != null){
				try {
					mmOutStream.write(b);
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
			
		}

		public void run() {
			InputStream in = mmInStream;
			byte[] buffer = new byte[Constants.BUFFER_SIZE];
			int count;
			while (!mmClosed.get()) {
				try {
					count = in.read(buffer);
					received(buffer, 0, count);
				} catch (IOException e) {
					connectionLost(e);
					cancel();
					break;
				}
			}
		}

		/*
		 * public void write(byte[] buffer) { try { mmOutStream.write(buffer); }
		 * catch (IOException e) { Log.e(Constants.TAG, "write failed", e); } }
		 */

		void connectionLost(IOException e) {
	//		dumpMessage(e.getLocalizedMessage());
		}

		void received(byte[] buffer, int offset, int count) {

			String str = new String(buffer, offset, count);
//			dumpReceivedMessage(Integer.toString(str.length()));

			serialPort_DataReceived(buffer, count);

		}
	}

	private final class ConnectTask implements Cancelable {

		private final AtomicBoolean mmClosed = new AtomicBoolean();
		private final BluetoothSocket mmSocket;

		public ConnectTask(BluetoothDevice device, UUID uuid) {
			BluetoothSocket socket = null;
			try {
				socket = device.createRfcommSocketToServiceRecord(uuid);
			} catch (IOException e) {
				Log.e(Constants.TAG, "create failed", e);
			}
			mmSocket = socket;
		}

		public void cancel() {
			if (mmClosed.getAndSet(true)) {
				return;
			}
			try {
				mmSocket.close();
			} catch (IOException e) {
				Log.e(Constants.TAG, "close failed", e);
			}
		}

		public void run() {
			if (mBluetooth.isDiscovering()) {
				mBluetooth.cancelDiscovery();
			}
			try {
				mmSocket.connect();
				connected(mmSocket);
			} catch (IOException e) {
//				connectionFailed(e);
				cancel();
			}
		}

		void connected(BluetoothSocket socket) {
			mLock.lock();
			try {
//				dumpMessage("connected");
				final ConnectedTask task = new ConnectedTask(socket);
				Cancelable canceller = new CancellingTask(mExec, task);
				mExec.execute(canceller);
				mConnectedTask = task;

			} finally {
				mLock.unlock();
			}
		}
/*
		void connectionFailed(IOException e) {
			dumpMessage(e.getLocalizedMessage());
		}
	*/}

	private static final String RECENT_DEVICE = "recent_device";
	private static final int REQUEST_SELECT_DEVICE = 0;

	private BluetoothAdapter mBluetooth;
	private ConnectedTask mConnectedTask;
	private ExecutorService mExec;
	private ReentrantLock mLock = new ReentrantLock();
/*
	public void dumpReceivedMessage(String str) {
		dumpMessage(str);
	}
*/
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.client, menu);
		return true;
	}

	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item) {
		int id = item.getItemId();
		switch (id) {
		case R.id.recent_device:
			BluetoothDevice recent = loadDefault();
			onDeviceSelected(recent);
			break;
		case R.id.search:
			Intent selectDevice = new Intent(this, DeviceListActivity.class);
			startActivityForResult(selectDevice, REQUEST_SELECT_DEVICE);
			break;
		default:
			return super.onMenuItemSelected(featureId, item);
		}
		return true;
	}

	@Override
	public boolean onMenuOpened(int featureId, Menu menu) {
		MenuItem item = menu.findItem(R.id.recent_device);
		BluetoothDevice recent = loadDefault();
		if (recent == null) {
			item.setVisible(false);
		} else {
			item.setTitle(recent.getName());
		}
		return super.onMenuOpened(featureId, menu);
	}

	public void rejectedExecution(Runnable r, ThreadPoolExecutor executor) {
		// noop.
	}

	private void consumeRequestDeviceSelect(int resultCode, Intent data) {
		if (resultCode != RESULT_OK) {
			return;
		}
		BluetoothDevice device = data
				.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
		saveAsDefault(device);
		onDeviceSelected(device);
	}
/*
	private void dumpMessage(final String msg) {
		Runnable dumpTask = new Runnable() {
			public void run() {
				//edit1.setText(msg);
			}
		};
		runOnUiThread(dumpTask);
	}
*/
	private BluetoothDevice loadDefault() {
		SharedPreferences prefs = PreferenceManager
				.getDefaultSharedPreferences(this);
		String addr = prefs.getString(RECENT_DEVICE, null);
		if (addr == null) {
			return null;
		}
		BluetoothDevice device = mBluetooth.getRemoteDevice(addr);
		return device;
	}

	private void onDeviceSelected(BluetoothDevice device) {
		mLock.lock();
		try {
			if (mConnectedTask != null) {
				mConnectedTask.cancel();
				mConnectedTask = null;
			}
		} finally {
			mLock.unlock();
		}
	//	dumpMessage("connecting");
		ConnectTask task = new ConnectTask(device,
				Constants.SERIAL_PORT_PROFILE);
		Cancelable canceller = new CancellingTask(mExec, task, 10,
				TimeUnit.SECONDS);
		mExec.execute(canceller);
	}

	private void saveAsDefault(BluetoothDevice device) {
		SharedPreferences prefs = PreferenceManager
				.getDefaultSharedPreferences(this);
		Editor editor = prefs.edit();
		String addr = device.getAddress();
		editor.putString(RECENT_DEVICE, addr);
		editor.commit();
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		switch (requestCode) {
		case REQUEST_SELECT_DEVICE:
			consumeRequestDeviceSelect(resultCode, data);
			return;
		}
		super.onActivityResult(requestCode, resultCode, data);
	}

	ImageView image1;
	Button button1;
	Button button2;
	Button button3;
	Button button4;


	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		mExec = Executors.newCachedThreadPool();
		((ThreadPoolExecutor) mExec).setRejectedExecutionHandler(this);
		mBluetooth = BluetoothAdapter.getDefaultAdapter();

		// Initialize Global Array
		header[0] = (byte) 'R';
		header[1] = (byte) 'X';
		header[2] = (byte) '=';

	//	edit1 = (EditText) findViewById(R.id.editText1);
	//	edit2 = (EditText) findViewById(R.id.editText2);
		image1 = (ImageView) findViewById(R.id.imageView1);

		button1 = (Button) findViewById(R.id.button1);
		button2 = (Button) findViewById(R.id.button2);
		button3 = (Button) findViewById(R.id.button3);
		button4 = (Button) findViewById(R.id.button4);
		
		
		button1.setOnTouchListener(new View.OnTouchListener() {
			
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				switch(event.getAction())
				{
				case MotionEvent.ACTION_UP :
					if (mConnectedTask != null)
					{
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)'o');
						mConnectedTask.sendOneByte((byte)'v');
						mConnectedTask.sendOneByte((byte)'e');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'p');
						mConnectedTask.sendOneByte((byte)'w');
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'\r');
						mConnectedTask.sendOneByte((byte)'\n');
					}
					break;
				case MotionEvent.ACTION_DOWN :
					if (mConnectedTask != null)
					{
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)'o');
						mConnectedTask.sendOneByte((byte)'v');
						mConnectedTask.sendOneByte((byte)'e');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'p');
						mConnectedTask.sendOneByte((byte)'w');
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'1');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'1');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'\r');
						mConnectedTask.sendOneByte((byte)'\n');
					}
					break;
				}
				return false;
			}
		});
		button2.setOnTouchListener(new View.OnTouchListener() {
			
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				switch(event.getAction())
				{
				case MotionEvent.ACTION_UP :
					if (mConnectedTask != null)
					{
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)'o');
						mConnectedTask.sendOneByte((byte)'v');
						mConnectedTask.sendOneByte((byte)'e');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'p');
						mConnectedTask.sendOneByte((byte)'w');
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'\r');
						mConnectedTask.sendOneByte((byte)'\n');
					}
					break;
				case MotionEvent.ACTION_DOWN :
					if (mConnectedTask != null)
					{
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)'o');
						mConnectedTask.sendOneByte((byte)'v');
						mConnectedTask.sendOneByte((byte)'e');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'p');
						mConnectedTask.sendOneByte((byte)'w');
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'-');
						mConnectedTask.sendOneByte((byte)'1');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'1');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'\r');
						mConnectedTask.sendOneByte((byte)'\n');
					}
					break;
				}
				return false;
			}
		});		
		
		button3.setOnTouchListener(new View.OnTouchListener() {
			
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				switch(event.getAction())
				{
				case MotionEvent.ACTION_UP :
					if (mConnectedTask != null)
					{
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)'o');
						mConnectedTask.sendOneByte((byte)'v');
						mConnectedTask.sendOneByte((byte)'e');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'p');
						mConnectedTask.sendOneByte((byte)'w');
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'\r');
						mConnectedTask.sendOneByte((byte)'\n');
					}
					break;
				case MotionEvent.ACTION_DOWN :
					if (mConnectedTask != null)
					{
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)'o');
						mConnectedTask.sendOneByte((byte)'v');
						mConnectedTask.sendOneByte((byte)'e');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'p');
						mConnectedTask.sendOneByte((byte)'w');
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'1');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'-');
						mConnectedTask.sendOneByte((byte)'1');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'\r');
						mConnectedTask.sendOneByte((byte)'\n');
					}
					break;
				}
				return false;
			}
		});		
		
		button4.setOnTouchListener(new View.OnTouchListener() {
			
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				switch(event.getAction())
				{
				case MotionEvent.ACTION_UP :
					if (mConnectedTask != null)
					{
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)'o');
						mConnectedTask.sendOneByte((byte)'v');
						mConnectedTask.sendOneByte((byte)'e');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'p');
						mConnectedTask.sendOneByte((byte)'w');
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'\r');
						mConnectedTask.sendOneByte((byte)'\n');
					}
					break;
				case MotionEvent.ACTION_DOWN :
					if (mConnectedTask != null)
					{
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)'o');
						mConnectedTask.sendOneByte((byte)'v');
						mConnectedTask.sendOneByte((byte)'e');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'p');
						mConnectedTask.sendOneByte((byte)'w');
						mConnectedTask.sendOneByte((byte)'m');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'-');
						mConnectedTask.sendOneByte((byte)'1');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)' ');
						mConnectedTask.sendOneByte((byte)'-');
						mConnectedTask.sendOneByte((byte)'1');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'0');
						mConnectedTask.sendOneByte((byte)'\r');
						mConnectedTask.sendOneByte((byte)'\n');
					}
					break;
				}
				return false;
			}
		});		
/*
		button1.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				
				if (mConnectedTask != null)
				{
					mConnectedTask.sendOneByte((byte)'m');
					mConnectedTask.sendOneByte((byte)'o');
					mConnectedTask.sendOneByte((byte)'v');
					mConnectedTask.sendOneByte((byte)'e');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'p');
					mConnectedTask.sendOneByte((byte)'w');
					mConnectedTask.sendOneByte((byte)'m');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'1');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'1');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'\r');
					mConnectedTask.sendOneByte((byte)'\n');
				}
			}
		});
		
		button2.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				
				if (mConnectedTask != null)
				{
					mConnectedTask.sendOneByte((byte)'m');
					mConnectedTask.sendOneByte((byte)'o');
					mConnectedTask.sendOneByte((byte)'v');
					mConnectedTask.sendOneByte((byte)'e');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'p');
					mConnectedTask.sendOneByte((byte)'w');
					mConnectedTask.sendOneByte((byte)'m');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'-');
					mConnectedTask.sendOneByte((byte)'1');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'1');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'\r');
					mConnectedTask.sendOneByte((byte)'\n');
				}
				
			}
		});
		
		button3.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				
				if (mConnectedTask != null)
				{
					mConnectedTask.sendOneByte((byte)'m');
					mConnectedTask.sendOneByte((byte)'o');
					mConnectedTask.sendOneByte((byte)'v');
					mConnectedTask.sendOneByte((byte)'e');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'p');
					mConnectedTask.sendOneByte((byte)'w');
					mConnectedTask.sendOneByte((byte)'m');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'1');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'-');
					mConnectedTask.sendOneByte((byte)'1');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'\r');
					mConnectedTask.sendOneByte((byte)'\n');
				}
				
			}
		});
		
		button4.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				
				if (mConnectedTask != null)
				{
					mConnectedTask.sendOneByte((byte)'m');
					mConnectedTask.sendOneByte((byte)'o');
					mConnectedTask.sendOneByte((byte)'v');
					mConnectedTask.sendOneByte((byte)'e');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'p');
					mConnectedTask.sendOneByte((byte)'w');
					mConnectedTask.sendOneByte((byte)'m');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'-');
					mConnectedTask.sendOneByte((byte)'1');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)' ');
					mConnectedTask.sendOneByte((byte)'-');
					mConnectedTask.sendOneByte((byte)'1');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'0');
					mConnectedTask.sendOneByte((byte)'\r');
					mConnectedTask.sendOneByte((byte)'\n');
				}
				
			}
		});
*/
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		mExec.shutdownNow();
	}

	// BCam//
	int MaxBuffer = 5000;
	byte[] buff = new byte[MaxBuffer];

	int MaxFIFO = 100000;
	byte[] m_pBuf = new byte[MaxFIFO];

	int frameCount = 0;
	int m_receiveMode = 0;
	int addr = 0;
	int ImageSize = 0;

	byte[] header = new byte[3];
	byte[] adcValue = new byte[8];
	byte DValue = 0x00;

	public int ByteIndexOf(byte[] searched, byte[] find, int start, int end) {
		// Do standard error checking here.
		Boolean matched = false;

		for (int index = start; index <= end - find.length; ++index) {
			// Assume the values matched.
			matched = true;

			// Search in the values to be found.
			for (int subIndex = 0; subIndex < find.length; ++subIndex) {
				// Check the value in the searched array vs the value
				// in the find array.
				if (find[subIndex] != searched[index + subIndex]) {
					// The values did not match.
					matched = false;

					// Break out of the loop.
					break;
				}
			}

			// If the values matched, return the index.
			if (matched) {
				// Return the index.
				return index;
			}
		}

		// None of the values matched, return -1.
		return -1;
	}

	byte[] BlockCopy(byte[] src, int start_ind, int length) {
		byte[] res = new byte[length];

		for (int i = 0; i < length; i++) {
			res[i] = src[i + start_ind];
		}

		return res;
	}

	final int CIRCLED_QUEUE_SIZE = 100000;

	byte[] _CQ_Array = new byte[CIRCLED_QUEUE_SIZE];
	int _s_array_ind = -1;
	int _e_array_ind = 0;

	void CQ_AddBytes(byte[] bytes, int count) {
		if (_s_array_ind < 0)
			_s_array_ind = 0;

		for (int i = 0; i < count; i++) {
			_CQ_Array[_e_array_ind] = bytes[i];

			_e_array_ind++;

			if (_e_array_ind >= CIRCLED_QUEUE_SIZE)
				_e_array_ind = 0;
		}
	}

	int CQ_GetLength() {
		if (_s_array_ind < 0 || _e_array_ind < 0)
			return 0;
		else if (_e_array_ind >= _s_array_ind)
			return (_e_array_ind - _s_array_ind);
		else {
			int len1 = CIRCLED_QUEUE_SIZE - _s_array_ind;
			int len2 = _e_array_ind;

			return len1 + len2;
		}
	}

	byte[] CQ_GetData(int length) {
		byte[] res = new byte[length];

		for (int i = 0; i < length; i++) {
			res[i] = _CQ_Array[_s_array_ind];

			_s_array_ind++;

			if (_s_array_ind >= CIRCLED_QUEUE_SIZE)
				_s_array_ind = 0;
		}

		return res;
	}

	byte[] CQ_GetData(int start_ind, int length) {
		byte[] res = new byte[length - start_ind];

		for (int i = 0; i < length; i++) {
			if (i >= start_ind)
				res[i - start_ind] = _CQ_Array[_s_array_ind];

			_s_array_ind++;

			if (_s_array_ind >= CIRCLED_QUEUE_SIZE)
				_s_array_ind = 0;
		}

		return res;
	}

	void CQ_RemoveData(int length) {
		for (int i = 0; i < length; i++) {
			_s_array_ind++;

			if (_s_array_ind >= CIRCLED_QUEUE_SIZE)
				_s_array_ind = 0;
		}
	}

	void CQ_ClearData() {
		_s_array_ind = -1;
		_e_array_ind = 0;
	}

	byte[] _FIFO_Array = new byte[CIRCLED_QUEUE_SIZE];
	int _s_fifo_ind = -1;
	int _e_fifo_ind = 0;

	void FIFO_AddBytes(byte[] bytes, int count) {
		if (_s_fifo_ind < 0)
			_s_fifo_ind = 0;

		for (int i = 0; i < count; i++) {
			_FIFO_Array[_e_fifo_ind] = bytes[i];

			_e_fifo_ind++;

			if (_e_fifo_ind >= CIRCLED_QUEUE_SIZE)
				_e_fifo_ind = 0;
		}
	}

	int FIFO_GetLength() {
		if (_s_fifo_ind < 0 || _e_fifo_ind < 0)
			return 0;
		else if (_e_fifo_ind >= _s_fifo_ind)
			return (_e_fifo_ind - _s_fifo_ind);
		else {
			int len1 = CIRCLED_QUEUE_SIZE - _s_fifo_ind;
			int len2 = _e_fifo_ind;

			return len1 + len2;
		}
	}

	byte[] FIFO_GetData(int start_ind, int length) {
		byte[] res = new byte[length - start_ind];

		for (int i = 0; i < length; i++) {
			if (i >= start_ind)
				res[i - start_ind] = _FIFO_Array[_s_fifo_ind];

			_s_fifo_ind++;

			if (_s_fifo_ind >= CIRCLED_QUEUE_SIZE)
				_s_fifo_ind = 0;
		}

		return res;
	}

	byte[] FIFO_GetData(int length) {
		byte[] res = new byte[length];

		for (int i = 0; i < length; i++) {
			res[i] = _FIFO_Array[_s_fifo_ind];

			_s_fifo_ind++;

			if (_s_fifo_ind >= CIRCLED_QUEUE_SIZE)
				_s_fifo_ind = 0;
		}

		return res;
	}

	void FIFO_RemoveData(int length) {
		for (int i = 0; i < length; i++) {
			_s_fifo_ind++;

			if (_s_fifo_ind >= CIRCLED_QUEUE_SIZE)
				_s_fifo_ind = 0;
		}
	}

	void FIFO_ClearData() {
		_s_fifo_ind = -1;
		_e_fifo_ind = 0;
	}

	public void serialPort_DataReceived(byte[] recv_buff, int recv_count) {
		try {
			CQ_AddBytes(recv_buff, recv_count);

			int fSize = 0;

			if (m_receiveMode == 0) {
				if (CQ_GetLength() < 50)
					return;

				fSize = CQ_GetLength();
				buff = CQ_GetData(fSize);

				int index = ByteIndexOf(buff, header, 0, fSize);

				if (index != -1) {
					ImageSize = 0;
					ImageSize = (int) (buff[index + 3] & 0x00ff) << 8;
					ImageSize = ImageSize | (buff[index + 4] & 0xff);

					addr = 0;
					int imageBase = index + 5;

					byte[] temp_buff = BlockCopy(buff, imageBase, fSize
							- imageBase);

					FIFO_AddBytes(temp_buff, fSize - imageBase);
					addr += (fSize - imageBase);

					m_receiveMode = 1;
				}
			} else if (m_receiveMode == 1) {

				if (addr < ImageSize) {
					fSize = CQ_GetLength();
					byte[] temp_buff = CQ_GetData(fSize);
					FIFO_AddBytes(temp_buff, fSize);

					addr += fSize;
				}

				if (addr >= ImageSize) {

					m_pBuf = FIFO_GetData(FIFO_GetLength());

					frameCount++;
					m_receiveMode = 0;

					for (int i = 0; i < 5; i++)
						adcValue[i] = m_pBuf[i];

					for (int i = ImageSize - 5, j = 5; i < ImageSize - 2; i++, j++) {
						adcValue[j] = m_pBuf[i];
					}

					DValue = m_pBuf[ImageSize - 2];

					String data_str = String
							.format("ADC data [%d %d %d %d %d %d %d %d] \r\nDigital Input data [%x]",
									adcValue[0], adcValue[1], adcValue[2],
									adcValue[3], adcValue[4], adcValue[5],
									adcValue[6], adcValue[7], DValue);
//					LogInfo(data_str);

					ShowJpegData();
				}

			}
		} catch (final Exception ex) {
//			LogInfo(ex.toString());
		}
	}
/*
	private void LogInfo(final String log) {
		Runnable dumpTask = new Runnable() {
			public void run() {
				//edit2.setText(log);
			}
		};
		runOnUiThread(dumpTask);
	}
*/
	private void ShowJpegData() {
		try {
			byte[] PictureData = new byte[ImageSize - 10];

			PictureData = BlockCopy(m_pBuf, 5, ImageSize - 10);

			final Bitmap bmp = BitmapFactory.decodeByteArray(PictureData, 0,
					PictureData.length);

			Runnable dumpTask = new Runnable() {
				public void run() {

					// Drawable old = image1.getDrawable();
					image1.setImageBitmap(bmp);
					// old = null;
				}
			};
			runOnUiThread(dumpTask);
		} catch (Exception ex) {
//			LogInfo(ex.toString());
		}
	}

}
