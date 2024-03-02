using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace DummyWindowsExe
{
    class Program
    {
        static void Main(string[] args)
        {
            using (EventLog eventLog = new EventLog("Application"))
            {
                eventLog.Source = "Application";
                eventLog.WriteEntry("DummyWindowsExe started", EventLogEntryType.Information, 0, 0);
            }

            Console.WriteLine("DummyWindowsExe started");

            while (true)
            {
                Thread.Sleep(1000);
            }
        }
    }
}
