using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO.Ports;
using System.Threading;
using System.IO;

namespace UserApplication
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private SerialPort? _serialPort;
        private Thread _readThread;
        private FileStream _outFile;
        private Mutex _mutex;

        public MainWindow()
        {
            InitializeComponent();

            Voltage.Text = "0.00000V";
            _serialPort = null;
            _readThread = new Thread(new ThreadStart(Read));
            _outFile = new FileStream("./data.txt", FileMode.Append);
            _mutex = new Mutex();

            _readThread.Start();
            RangeSelector.Items.Add("200mV");
            RangeSelector.Items.Add("2V");
            RangeSelector.Items.Add("20V");

            AllocConsole();
        }

        ~MainWindow()
        {
            FreeConsole();
            if (_serialPort != null)
                _serialPort.Close();
        }

        private void SetRangeMux_Click(object sender, RoutedEventArgs e)
        {
            if (_serialPort == null)
                return;

            if (Bit12.IsChecked == true && Bit13.IsChecked == true)
            {
                MessageBox.Show("Warning: Bits 12 and 13 are both set!\nCancelling Operation.", "Danger", MessageBoxButton.OK, MessageBoxImage.Stop);
            }
            else if (Bit14.IsChecked == true && Bit15.IsChecked == true)
            {
                MessageBox.Show("Warning: Bits 14 and 14 are both set!\nCancelling Operation.", "Danger", MessageBoxButton.OK, MessageBoxImage.Stop);
            }

            ushort value = 0;
            if (Bit0.IsChecked == true) value |= (1 << 0);
            if (Bit1.IsChecked == true) value |= (1 << 1);
            if (Bit2.IsChecked == true) value |= (1 << 2);
            if (Bit3.IsChecked == true) value |= (1 << 3);
            if (Bit4.IsChecked == true) value |= (1 << 4);
            if (Bit5.IsChecked == true) value |= (1 << 5);
            if (Bit6.IsChecked == true) value |= (1 << 6);
            if (Bit7.IsChecked == true) value |= (1 << 7);

            if (Bit12.IsChecked == true) value |= (1 << 12);
            if (Bit13.IsChecked == true) value |= (1 << 13);
            if (Bit14.IsChecked == true) value |= (1 << 14);
            if (Bit15.IsChecked == true) value |= (1 << 15);

            byte[] command = new byte[2] { 0x00, (byte)(value & 0xFF) };
            _serialPort.Write(command, 0, command.Length);
            Console.WriteLine($"Dispatched MUX Command as {value & 0xFF}");
        }

        private void SetRefVoltage_Click(object sender, RoutedEventArgs e)
        {
            if (_serialPort == null)
                return;

            if (float.TryParse(RefVoltage.Text, out float voltage))
            {
                byte[] cmd = new byte[1] { 0x03 };
                var command = cmd.Concat(BitConverter.GetBytes(voltage)).ToArray()!;

                _serialPort.Write(command, 0, command.Length);
                Console.WriteLine($"Dispatched RefVoltage Command at {voltage}V");
            }
        }

        private void SetAutoZeroButton_Click(object sender, RoutedEventArgs e)
        {
            if (_serialPort == null)
                return;
        }

        private bool IsValidComPort()
        {
            foreach (string port in SerialPort.GetPortNames())
            {
                if (port == ComPort.Text)
                {
                    return true;
                }
            }

            return false;
        }

        private void RangeSelector_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (_serialPort == null)
                return;

            byte[] command = new byte[2] { 0x02, (byte)RangeSelector.SelectedIndex };
            _serialPort.Write(command, 0, command.Length);
            Console.WriteLine($"Dispatched Range Command for {(string)RangeSelector.SelectedItem}");
        }

        private void ConnectToDvm_Click(object sender, RoutedEventArgs e)
        {
            if (ComPort.Text != "")
            {
                if (!IsValidComPort())
                {
                    StatusLabel.Foreground = new SolidColorBrush(Color.FromRgb(0xEE, 0, 0));
                    StatusLabel.Content = "Failed";

                    Console.WriteLine($"Failed to connect: Invalid COM Port '{ComPort.Text}'");
                    return;
                }

                if (_serialPort != null)
                {
                    _serialPort.Close();
                }

                _serialPort = new SerialPort(ComPort.Text, 115200);
                _serialPort.Open();

                StatusLabel.Foreground = new SolidColorBrush(Color.FromRgb(0, 0xEE, 0));
                StatusLabel.Content = "Success";
                Console.WriteLine($"Successfully connected to COM Port '{ComPort.Text}'");
            }
        }

        public static string GetTimestamp(DateTime value)
        {
            return value.ToString("yyyyMMddHHmmssffff");
        }

        private void Read()
        {
            StreamWriter writer = new(_outFile);
            while (true)
            {
                if (_serialPort != null && _serialPort.IsOpen)
                {
                    if (_serialPort.BytesToRead >= 5)
                    {
                        byte[] data = new byte[5];
                        _serialPort.Read(data, 0, data.Length);
                        
                        if (data[0] == 0x00)
                        {
                            float value = BitConverter.ToSingle(data, 1);
                            Voltage.Text = value.ToString();

                            string output = $"[{GetTimestamp(DateTime.Now)}] {value}";

                            Console.WriteLine(output);
                            if (SaveValuesInFile.IsChecked == true)
                            {
                                writer.WriteLine(output);
                            }
                        }
                    }
                }

                Thread.Sleep(10);
            }
        }

        [DllImport("Kernel32")]
        public static extern void AllocConsole();

        [DllImport("Kernel32")]
        private static extern void FreeConsole();

        
    }
}
