using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
using System.Web;

namespace GeocitiesTest
{
	class MainClass
	{
		const string inputDir = @"/media/minhdanh/EXT4_BKUP/geocities/Geocities Accounts";
		const string outputDir = @"/media/minhdanh/NTFS_BKUP1/temp/geocities";
		const int targetTotalFiles = 100000000;

		static long countCopied = 0;
		static long countIgnored = 0;
		static long countErrors = 0;
		static long countExists = 0;
		static long countTotal = 0;
		static long countHtml = 0;
		static long countTxt = 0;
		static DateTime startDate = DateTime.MinValue;

		public static string CalculateMD5Hash(string input)
		{
			MD5 md5 = MD5.Create();
			byte[] inputBytes = System.Text.Encoding.ASCII.GetBytes(input);
			byte[] hash = md5.ComputeHash(inputBytes);

			StringBuilder sb = new StringBuilder();
			for (int i = 0; i < hash.Length; i++)
			{
				sb.Append(hash[i].ToString("X2"));
			}
			return sb.ToString();
		}
			
		static string stripHTML(string inputHTML)
		{
			string lineMarker = "****";

			inputHTML = inputHTML.Replace("\r\n", "\n");
			inputHTML = inputHTML.Replace("\n\r", "\n");
			inputHTML = inputHTML.Replace("\r", "\n");
			inputHTML = inputHTML.Replace((char)160, '\n'); // space
			inputHTML = inputHTML.Replace("\n", lineMarker);
			inputHTML = Regex.Replace(inputHTML, @"<style.*?</style>", " ").Trim();
			inputHTML = Regex.Replace(inputHTML, @"<script.*?</script>", " ").Trim();
			inputHTML = Regex.Replace(inputHTML, @"<(.|\n)*?>", " ").Trim();
			inputHTML = HttpUtility.HtmlDecode(inputHTML);
			inputHTML = inputHTML.Replace(lineMarker, "\n");
			// inputHTML = StripPunctuation(inputHTML).Trim();

			var paragraphs = inputHTML.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
			StringBuilder sb = new StringBuilder();
			for (int i = 0; i < paragraphs.Length; i++)
			{
				string temp = paragraphs[i].Trim();
				temp = Regex.Replace(temp, @"\s{2,}", " ");
				if (temp.Length > 0)
				{
					sb.Append(temp);

					if (i < paragraphs.Length - 1)
					{
						sb.Append("\n");
					}
				}
			}

			return sb.ToString();
		}


		public static void DirSearch(string sDir, string[] patterns)
		{
			foreach (string d in Directory.GetDirectories(sDir))
			{
				foreach (var pattern in patterns)
				{
					foreach (string f in Directory.GetFiles(d, pattern))
					{
						// Console.WriteLine(f);
						FileInfo fi = new FileInfo(f);

						bool isSelected = false;
						bool isHtml = false;
						String fileExt = Path.GetExtension (f).ToLower ();
						switch(fileExt)
						{
						case ".html":
						case ".htm":
							isSelected = (fi.Length >= 4000 && fi.Length <= 4000000);
							isHtml = true;
							countHtml++;
							break;
						case ".txt":
							isSelected = (fi.Length >= 2000 && fi.Length <= 2000000);
							countTxt++;
							break;
						}

						if (isSelected) {
							String fileMD5 = CalculateMD5Hash (f);
							String fileDir = outputDir + "/" + fileMD5 [0] + "/" + fileMD5 [1] + "/" + fileMD5 [2] + "/" + fileMD5 [3] + "/" + fileMD5 [4];
							if (!Directory.Exists (fileDir))
								Directory.CreateDirectory (fileDir);

							String newFileName = fileDir + "/" + fileMD5 + ".txt";
							try {
								if (!File.Exists (newFileName)) {
									if (isHtml) {
										String htmlContents = File.ReadAllText (f);
										String textContents = stripHTML (htmlContents);
										File.WriteAllText (newFileName, textContents);
									} else {
										File.Copy (f, newFileName);
									}
									countCopied++;
								} else {
									countExists++;
								}
							} catch (Exception ex) {
								Console.WriteLine (f + ": " + ex.Message);
								countErrors++;
							}

							if (countTotal % 100 == 0 || countTotal >= targetTotalFiles) {
								writeResults ();
							}

							countTotal++;

							if (countTotal >= targetTotalFiles) {
								break;
							}
						} else {
							countIgnored++;
						}
					}
				}

				if (countTotal >= targetTotalFiles)
				{
					break;
				}
				else 
					DirSearch(d, patterns);
			}
		}

		public static void writeResults()
		{
			DateTime currentDate = DateTime.Now;
			long lapsed = (long)(currentDate - startDate).TotalSeconds;
			Console.WriteLine (String.Format ("{0} - Created: {1}, Exists: {2}, Errors: {3}, Target: {4}. Html: {5}, Txt: {6}. Ignored: {7}. Lapsed: {8}s", 
												currentDate, countCopied, countExists, countErrors, targetTotalFiles, countHtml, countTxt, countIgnored, lapsed)); 
		}

		public static void Main (string[] args)
		{
			startDate = DateTime.Now;

			DirSearch(inputDir, new string[] { "*.htm", "*.html", "*.txt"});

			writeResults();
			Console.WriteLine ("Done");
			Console.ReadLine();
		}
	}
}
