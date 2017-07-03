using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Benchmark
{
    class Program
    {
        static readonly int[] Values = 
        {
	        50,	100, 200, 300, 500,	750,
	        1000, 1500, 2000, 2500,	3000, 3500,	4000, 4500, 5000,
	        6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000,
	        16000, 17000, 18000, 19000, 20000, 21000, 22000, 23000, 24000, 25000
        };

        static Dictionary<string, Dictionary<int, Tuple<double, int>>> benchmarkResults;

        static void Main(string[] args)
        {
            benchmarkResults = new Dictionary<string, Dictionary<int, Tuple<double, int>>>();
            benchmarkResults["Merge sort with random access"] = new Dictionary<int, Tuple<double, int>>();
            benchmarkResults["Heap sort with random access"] = new Dictionary<int, Tuple<double, int>>();
            benchmarkResults["Radix sort with random access"] = new Dictionary<int, Tuple<double, int>>();
            benchmarkResults["Merge sort with sequencial access"] = new Dictionary<int, Tuple<double, int>>();
            benchmarkResults["Heap sort with sequencial access"] = new Dictionary<int, Tuple<double, int>>();
            benchmarkResults["Radix sort with sequencial access"] = new Dictionary<int, Tuple<double, int>>();

            foreach (var value in Values)
            {
                Test(value);
            }

            using (var writer = new StreamWriter("Benchmark.txt"))
            {
                foreach (var algorithm in benchmarkResults)
                {
                    writer.WriteLine(algorithm.Key + ":");
                    writer.WriteLine();

                    foreach (var result in algorithm.Value)
                    {
                        writer.WriteLine("{0}\t{1:0.0000000}\t{2}", result.Key, result.Value.Item1, result.Value.Item2);
                    }

                    writer.WriteLine();
                    writer.WriteLine();
                }
            }
        }

        static void Test(int value)
        {
            Console.WriteLine("Testing with n = {0}:", value);

            var startInfo = new ProcessStartInfo();

            startInfo.Arguments = "--test";
            startInfo.CreateNoWindow = true;
            startInfo.RedirectStandardInput = true;
            startInfo.RedirectStandardOutput = true;
            startInfo.UseShellExecute = false;
            startInfo.FileName = "LabSort.exe";

            Console.Write("Creating process... ");
            using (var process = Process.Start(startInfo))
            {
                Console.WriteLine("Done!");

                Console.Write("Writing to input... ");
                process.StandardInput.WriteLine(value.ToString());
                Console.WriteLine("Done!");

                Console.Write("Waiting for exit... ");
                process.WaitForExit();
                Console.WriteLine("Done!\r\n");

                benchmarkResults["Merge sort with random access"][value] = ParseDoubleIntTuple(process.StandardOutput.ReadLine());
                benchmarkResults["Heap sort with random access"][value] = ParseDoubleIntTuple(process.StandardOutput.ReadLine());
                benchmarkResults["Radix sort with random access"][value] = ParseDoubleIntTuple(process.StandardOutput.ReadLine());
                benchmarkResults["Merge sort with sequencial access"][value] = ParseDoubleIntTuple(process.StandardOutput.ReadLine());
                benchmarkResults["Heap sort with sequencial access"][value] = ParseDoubleIntTuple(process.StandardOutput.ReadLine());
                benchmarkResults["Radix sort with sequencial access"][value] = ParseDoubleIntTuple(process.StandardOutput.ReadLine());
            }
        }

        static Tuple<double, int> ParseDoubleIntTuple(string str)
        {
            var members = str.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

            return new Tuple<double, int>(double.Parse(members[0], CultureInfo.InvariantCulture), int.Parse(members[1]));
        }
    }
}
