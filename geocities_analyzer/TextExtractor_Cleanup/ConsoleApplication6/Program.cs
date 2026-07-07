using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml.Serialization;
using System.Text.RegularExpressions;
using System.Diagnostics;
using System.Threading;

namespace ConsoleApplication6
{
    class Program
    {        
        static string outputParaFolder = @"C:\temp\onlyThoughts_retVal_7mil";

        static List<String> paraContent = new List<string>();
        static List<List<string>> paraTopics = new List<List<string>>();
        const int threadCount = 24;

        static void Main(string[] args)
        {
            var inputPara = @"C:\temp\onlyThoughts_retVal\retValByContent_content.xml";
            var inputParaTxt = @"C:\temp\onlyThoughts_retVal_7mil\selected_para.txt";
            var inputTopics = @"C:\temp\onlyThoughts_retVal\retValByContent_topics.xml";
            var outputPara = @"C:\temp\onlyThoughts_retVal_7mil\output_para.txt";

            // prepare data structure
            Console.WriteLine(DateTime.Now + " - Reading data files ...");
            if (File.Exists(outputPara))
                File.Delete(outputPara);

            // read content from text file
            Console.WriteLine(DateTime.Now + " - Reading " + inputParaTxt);
            StreamReader sr = File.OpenText(inputParaTxt);
            while (!sr.EndOfStream)
            {
                String line = sr.ReadLine().Trim();
                if (line.Length > 0)
                {
                    paraContent.Add(line);

                    if (paraContent.Count % 500000 == 0)
                    {
                        Console.WriteLine(DateTime.Now + " - Records: " + paraContent.Count);
                    }
                }
            }
            sr.Close();
            Console.WriteLine(DateTime.Now + " - Records: " + paraContent.Count);

            // read content from XML
            /*
            Console.WriteLine(DateTime.Now + " - Reading " + inputPara);
            String fileContent = ReplaceHexadecimalSymbols(File.ReadAllText(inputPara));
            XmlSerializer serializer = new XmlSerializer(paraContent.GetType());
            using (TextReader reader = new StringReader(fileContent))
            {
                paraContent = (List<String>)serializer.Deserialize(reader);
                Console.WriteLine(DateTime.Now + " - Records: " + paraContent.Count);
            }
            */

            // read topics
            /*
            Console.WriteLine(DateTime.Now + " - Reading " + inputTopics);
            fileContent = ReplaceHexadecimalSymbols(File.ReadAllText(inputTopics));
            serializer = new XmlSerializer(paraTopics.GetType());
            using (TextReader reader = new StringReader(fileContent))
            {
                paraTopics = (List<List<string>>)serializer.Deserialize(reader);
                Console.WriteLine(DateTime.Now + " - Records: " + paraTopics.Count);
            }
            */

            // output format

            // single pass
            // processParagraph(paraContent, paraTopics, outputPara);

            // multiple threads
            for (int i = 0; i < threadCount; i++)
            {
                Thread thread = new Thread(new ParameterizedThreadStart(StartProcessText));
                thread.Start(i);
            }

            // done
            Console.ReadLine();
        }

        static void StartProcessText(object threadInd)
        {
            int i = (int)threadInd;
            int paraPerThread = (int)(paraContent.Count / threadCount);

            int paraStart = i * paraPerThread;
            int paraSkipped = i * (paraPerThread - 1);
            int paraEnd = Math.Min(paraContent.Count, paraStart + paraPerThread);
            String outputFile = outputParaFolder + "\\para_" + i + ".txt";
            Console.WriteLine(String.Format("Thread #{0} - From paragraph #{1} to #{2} saving to {3}", i, paraStart, paraEnd, outputFile));

            List<string> paraForThreads = paraContent.Skip(paraSkipped).Take(paraPerThread).ToList();
            if (File.Exists(outputFile))
                File.Delete(outputFile);

            processParagraph(i, paraForThreads, new List<List<string>>(), outputFile);
        }

        static void processParagraph(int threadInd, List<string> thisParaContent, List<List<string>> thisParaTopic, string outputParaFile)
        {
            char[] puncChars = { '.', ',', '?', '!', ':', '|' };
            char[] spaceChars = new char[] { ' ', (char)160 };

            string[] ignoredWordList = File.ReadAllLines("ignored.txt").Where(x => !string.IsNullOrEmpty(x)).Select(x => x.Trim()).Distinct().ToArray();
            string[] preferredWordList = File.ReadAllLines("preferred.txt").Where(x => !string.IsNullOrEmpty(x)).Select(x => x.Trim()).Distinct().ToArray();
            string[] avoidedWordList = File.ReadAllLines("avoided.txt").Where(x => !string.IsNullOrEmpty(x)).Select(x => x.Trim()).Distinct().ToArray();

            string[] moodHappyList = File.ReadAllLines("moodHappy.txt").Where(x => !string.IsNullOrEmpty(x)).Select(x => x.Trim()).Distinct().ToArray();
            string[] moodSadList = File.ReadAllLines("moodSad.txt").Where(x => !string.IsNullOrEmpty(x)).Select(x => x.Trim()).Distinct().ToArray();
            string[] moodIncreaseList = File.ReadAllLines("moodIncrease.txt").Where(x => !string.IsNullOrEmpty(x)).Select(x => x.Trim()).Distinct().ToArray();
            string[] moodDecreaseList = File.ReadAllLines("moodDecrease.txt").Where(x => !string.IsNullOrEmpty(x)).Select(x => x.Trim()).Distinct().ToArray();
            string[] personalList = File.ReadAllLines("personal.txt").Where(x => !string.IsNullOrEmpty(x)).Select(x => x.Trim()).Distinct().ToArray();

            Console.WriteLine(DateTime.Now + " - #" + threadInd + " Writing: " + outputParaFile);
            StringBuilder sbParaContent = new StringBuilder();
            sbParaContent.Append("para_index,content,number_count,name_count,exclamation_count,colon_count,mixed_case_count,question_count,word_count,phrase_count,personal_count,length,mood_score,keywords" + Environment.NewLine);

            int countParaWritten = 0;
            for (int para_index = 0; para_index < thisParaContent.Count; para_index++)
            {
                if (countParaWritten % 500 == 0)
                    Console.WriteLine(String.Format("{0} - #{1} {2} / {3} paragraph written ({4:0.00}%)", DateTime.Now, threadInd, countParaWritten, thisParaContent.Count, countParaWritten * 100.0 / thisParaContent.Count));

                var para = thisParaContent[para_index];

                int mixed_case_count = 0;
                int number_count = 0;

                int name_count = 0;

                int exclamation_count = 0;
                int colon_count = 0;
                int question_count = 0;

                double paraMood = 0;

                int phraseCount = 0;
                int wordCount = 0;

                int personalCount = 0;

                string keywords = "";

                try
                {
                    /*
                    var paraTags = paraTopics[para_index];
                    foreach (var tag in paraTags)
                    {
                        keywords += tag + ";";
                    }
                     */

                    exclamation_count = para.Count(c => c == '!');
                    colon_count = para.Count(c => c == ':');
                    question_count = para.Count(c => c == '?');

                    var paraSentences = para.Split(puncChars, StringSplitOptions.RemoveEmptyEntries);
                    foreach (var sentence in paraSentences)
                    {
                        var words = sentence.Split(spaceChars, StringSplitOptions.RemoveEmptyEntries);

                        for (int i = 0; i < words.Length; i++)
                        {
                            var word = words[i].Trim();
                            if (word.Length == 0)
                                continue;

                            if (i > 0 && word.Length >= 3 && word != word.ToLower())
                            {
                                name_count++;
                            }

                            if (moodHappyList.Where(x => String.Compare(x.ToLower(), word.ToLower()) == 0).ToList().Count() > 0)
                            {
                                paraMood++;
                            }
                            else if (moodSadList.Where(x => String.Compare(x.ToLower(), word.ToLower()) == 0).ToList().Count() > 0)
                            {
                                paraMood--;
                            }
                            else if (moodIncreaseList.Where(x => String.Compare(x.ToLower(), word.ToLower()) == 0).ToList().Count() > 0)
                            {
                                paraMood = paraMood * 1.1;
                            }
                            else if (moodDecreaseList.Where(x => String.Compare(x.ToLower(), word.ToLower()) == 0).ToList().Count() > 0)
                            {
                                paraMood = paraMood / 1.1;
                            }

                            if (personalList.Where(x => String.Compare(x.ToLower(), word.ToLower()) == 0).ToList().Count() > 0)
                            {
                                personalCount++;
                            }

                            if (word.Where(x => Char.IsDigit(x)).Any())
                            {
                                number_count++;
                            }

                            // mixed case word are usually some abbrev. or special terms (e.g. iOS, PhD, etc.)
                            if (word.Length >= 3)
                            {
                                string temp = Char.ToLower(word[0]) + word.Substring(1);
                                if (temp.ToLower() != temp && temp.ToUpper() != temp)
                                {
                                    mixed_case_count++;
                                }
                            }

                            wordCount++;
                        }
                        phraseCount++;
                    }

                    // mood increase or decrease depending on the punctuation in the paragraph
                    paraMood = paraMood * (question_count + 1);
                    paraMood = paraMood * (exclamation_count + 1);
                    paraMood = 100 * paraMood;
                }
                catch (Exception ex)
                {
                    Console.WriteLine(DateTime.Now + " - #" + threadInd + ": " + ex.Message);
                }

                sbParaContent.Append(String.Format("{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13}{14}",
                                                    para_index, StringToCSVCell(formatPunctuation(para).Trim()), number_count, name_count, exclamation_count,
                                                    colon_count, mixed_case_count, question_count,
                                                    wordCount, phraseCount, personalCount, para.Length, (int)paraMood, StringToCSVCell(keywords),
                                                    Environment.NewLine));

                countParaWritten++;
            }
            File.AppendAllText(outputParaFile, sbParaContent.ToString());
            Console.WriteLine(DateTime.Now + " - #" + threadInd + " " + String.Format("Done. {0} / {1} paragraphs written", countParaWritten, thisParaContent.Count));
        }

        static void Main2(string[] args)
        {
            var inputDir = @"C:\temp\paragraph_data";
            var outputFile = @"C:\temp\selected_para.txt";

            if (File.Exists(outputFile))
                File.Delete(outputFile);

            List<String> temp = new List<string>();

            var files = Directory.GetFiles(inputDir, "*.xml");
            foreach (var file in files)
            {
                Console.WriteLine(DateTime.Now + " - Reading: " + file);

                String fileContent = File.ReadAllText(file);
                String fileContentFiltered = ReplaceHexadecimalSymbols(fileContent);

                Console.WriteLine(DateTime.Now + " - Parsing: " + file);

                XmlSerializer serializer = new XmlSerializer(temp.GetType());
                using (TextReader reader = new StringReader(fileContentFiltered))
                {
                    List<String> thisSet = (List<String>)serializer.Deserialize(reader);

                    Console.WriteLine(DateTime.Now + " - Writing: " + outputFile);
                    StringBuilder sb = new StringBuilder();
                    foreach (var paragraph in thisSet)
                    {
                        String paraNew = Regex.Replace(paragraph, @"[^\u0000-\u007F]+", "");
                        sb.Append(paraNew + Environment.NewLine);
                    }
                    File.AppendAllText(outputFile, sb.ToString(), Encoding.UTF8);
                    Console.WriteLine(DateTime.Now + " - Records: " + thisSet.Count);
                }               
            }            

            Console.WriteLine(DateTime.Now + " - Done. Press ENTER to quit.");
            Console.ReadLine();
        }


        static string formatPunctuation(string input)
        {
            var pattern = new Regex(@"\s+(\p{P})");
            var newString = pattern.Replace(input, "$1"); // remove space before punctuation           
            newString = Regex.Replace(newString, @"\.(?! |$)", ". "); // add space after every punctuations except those that already have space
            newString = Regex.Replace(newString, @"\,(?! |$)", ", ");

            // can't replace question mark, because the accent mark in "của" will be replaced as well, later it will cause EncoderFallbackException 
            // See http://stackoverflow.com/questions/10377189/encoderfallbackexception-while-writing-to-file - unicode surrogate pair mismatch when writing file
            // newString = Regex.Replace(newString, @"\\?(?! |$)", "? "); 

            newString = Regex.Replace(newString, @"\:(?! |$)", ": ");
            return newString;
        }

        static string StringToCSVCell(string str)
        {
            bool mustQuote = (str.Contains(",") || str.Contains("\"") || str.Contains("\r") || str.Contains("\n"));
            if (mustQuote)
            {
                StringBuilder sb = new StringBuilder();
                sb.Append("\"");
                foreach (char nextChar in str)
                {
                    sb.Append(nextChar);
                    if (nextChar == '"')
                        sb.Append("\"");
                }
                sb.Append("\"");
                return sb.ToString();
            }

            return str;
        }

        static string getMD5Hash(string input)
        {
            StringBuilder sb = new StringBuilder();
            using (System.Security.Cryptography.MD5 md5 = System.Security.Cryptography.MD5.Create())
            {
                byte[] retVal = md5.ComputeHash(Encoding.Unicode.GetBytes(input));
                for (int i = 0; i < retVal.Length; i++)
                {
                    sb.Append(retVal[i].ToString("x2"));
                }
            }
            return sb.ToString();
        }

        static string ReplaceHexadecimalSymbols(string txt)
        {
            string r = "[\x00-\x08\x0B\x0C\x0E-\x1F\x26]";
            return Regex.Replace(txt, r, "", RegexOptions.Compiled);
        }
    }
}
