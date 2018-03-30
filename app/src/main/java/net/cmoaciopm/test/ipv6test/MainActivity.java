package net.cmoaciopm.test.ipv6test;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkInfo;
import android.os.Build;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Enumeration;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class MainActivity extends AppCompatActivity {

   private static final String TAG = "abc";
   private static final String FORMAT_NETWORK_INTERFACE = "%s : loopback = %b, p2p = %b, up = %b, virtual = %b";
   private static final String WELL_KNOWN_IPV4_ONLY = "ipv4only.arpa";

   // Used to load the 'native-lib' library on application startup.
   static {
      System.loadLibrary("native-lib");
   }

   @Override
   protected void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      setContentView(R.layout.activity_main);

      // Example of a call to a native method
      TextView tv = (TextView) findViewById(R.id.sample_text);
      tv.setText(stringFromJNI());

      ExecutorService executor = Executors.newSingleThreadExecutor();
      executor.execute(new Runnable() {
         @Override
         public void run() {
            printAllNetworkInterfaces();

            printNat64Prefix();

            nativeResolveHost("www.google.com");
         }
      });


   }

   private void printAllNetworkInterfaces() {
      try {
         Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();
         if (interfaces == null) {
            Log.d(TAG, "No network interface could be found!");
            return;
         }
         while (interfaces.hasMoreElements()) {
            NetworkInterface networkInterface = interfaces.nextElement();
            Log.d(TAG, String.format(FORMAT_NETWORK_INTERFACE,
                    networkInterface.toString(),
                    networkInterface.isLoopback(),
                    networkInterface.isPointToPoint(),
                    networkInterface.isUp(),
                    networkInterface.isVirtual()));

            Enumeration<InetAddress> addresses = networkInterface.getInetAddresses();
            if (addresses == null) {
               Log.d(TAG, "No address could be found for this interface!");
               return;
            }
            while (addresses.hasMoreElements()) {
               InetAddress address = addresses.nextElement();
               Log.d(TAG, address + "");
            }
         }
      } catch (SocketException e) {
         e.printStackTrace();
      }
   }

   private void printNat64Prefix() {
      String hostname = WELL_KNOWN_IPV4_ONLY;
      if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
         try {
            InetAddress[] addresses = InetAddress.getAllByName(hostname);
            for (int i = 0; i < addresses.length; i++) {
               Log.d(TAG, addresses[i] + "");
            }
         } catch (UnknownHostException e) {
            e.printStackTrace();
         }
      } else {
         ConnectivityManager manager = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
         Network[] networks = manager.getAllNetworks();
         for (int i = 0; i < networks.length; i++) {
            Network network = networks[i];
            NetworkInfo networkInfo = manager.getNetworkInfo(network);
            Log.d(TAG, networkInfo + "");

            try {
               InetAddress[] addresses = network.getAllByName(hostname);
               for (int j = 0; j < addresses.length; j++) {
                  Log.d(TAG, addresses[j] + "");
               }
            } catch (UnknownHostException e) {
               e.printStackTrace();
            }

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
               nativePrintIPv6Prefix(network.getNetworkHandle(), hostname);
            }
         }
      }
   }


   private void nativeResolveHost(String host) {
      nativeResolve(host);
   }

   /**
    * A native method that is implemented by the 'native-lib' native library,
    * which is packaged with this application.
    */
   public native String stringFromJNI();

   public native void nativePrintIPv6Prefix(long handle, String host);

   public native void nativeResolve(String host);
}
