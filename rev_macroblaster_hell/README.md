# srdnlenctf 2022

## Macroblaster Hell (X solves)

Do you think you are really good at reversing VBA macros?

Then the suffering caused by this challenge will be limited.

Maybe.

The flag is divided into three parts, which will make sense when combined. Enclose the final result in srdnlen{} to solve the challenge. And be careful of nasty traps.

A generous hint: Windows will most likely help.

### Solution

### Intro

We are given a DOC file. If we try to open it, we are presented with a Message saying "Sorry, too late."

This tells us that a macro is probably being executed at startup. If we inspect macros (Click on the "View" tab, click on "Macros"), we see that there is a Document_Open that in turn runs 2 procedures.

We will come back later on the first one; the second one however closes the document and deletes a figure and a text section, then closes the document.
Probably we need to open a untouched version of the file and stop preemptively the execution of the macro.

We have two ways: either we set the security policy on "Disable all macros" in the Microsoft Trust Center


or we open a protected instance with the following Python script:

    import win32com.client
    application = win32com.client.DispatchEx("Word.Application")
    application.Visible = True
    application.Application.AutomationSecurity = 3
    application.Documents.Open("path/to/file/challenge.doc")

Now that we have a Word instance where macros won't run, we can start the analysis.

Hint: using Libre Office simplifies this step, as the macro is never directly executed.

### First part of the flag

We saw that the code in the `JdabjnkvrewncaS` Sub deletes a Shape. We however cannot see any figure inside the document. Luckily, Shapes and their properties can be accessed via VBA. To run VBA Code, however, we need to open this file in a non-protected instance. We cannot do this if `JdabjnkvrewncaS` is run, because the document will be closed. So, we comment the call of this procedure in the Document_Open with `'`, we Save and we open another instance, this time not protected. We can again use the Trust Center or the following Python script:

    import win32com.client
    application = win32com.client.DispatchEx("Word.Application")
    application.Visible = True
    application.Application.AutomationSecurity = 1
    application.Documents.Open("path/to/file/challenge_modified.doc")

To inspect the shape property, we can use a MsgBox right after "Sorry too late". The following script will get the "Text" from the Shape:

For Ms Office:

    Msgbox ActiveDocument.Shapes(1).TextFrame.TextRange.Text

For Libre Office:
ActiveDocument.Shapes(1).Height = 100
ActiveDocument.Shapes(1).Width = 100

And check the enlarged figure in the page

We can see that the first part of the flag is `1nfect`

### Second part of the flag

The other macro, `Jpd522flx0et3_5xy1`, contains a very long code that employs various variables. We can immediately see that most of them are replicated code. In particular, there is a set of variables which seems a bit different from the others. If we go at lines 178->180, we see that these variables are a bit different. If we decode them as Base64 Strings, we get, respectively:

    Q29uZ3JhdHM= -> Congrats
    V2FzIGl0IGZ1biB0byBkZWNvZGU/ -> Was it fun to decode?
    MW9uX2NoNGlu -> 1on_ch4in

The second part of the flag is the last string: `1on_ch4in`

### Third part of the flag: the Powershell

Immediately before the fake code containing the variables hiding the second piece, we see there's a `CreateObject("WScript.Shell").Run` call (function Jpd522flx0et3_5xy1) . This is the only Object whose name is written in cleartext in the whole macro, and the Run method called is used to execute commands. We are probably interested in the argument of this function. Unfortunately, it does not seem to be written inside the code directly. If we use the VBA debugger, however, we can retrieve it. If we use a breakpoint to stop the execution and see the content of the argument (We can slightly alter the code and save the argument inside a variable before passing it to CreateObject.Run - for example line 167), we see that the content is

    VariableWeWantToSee : "poWershELl -w HiDdeN -command \path\to\evil.ps1" : Variant/String

The `Kill` instruction immediately after deletes this PS1 file by pathname. We can simply copy the `evil.ps1` in a different path to keep it.

### Third part of the flag: the Obfuscation

The script inside `evil.ps1` is, as is the rule with malware, obfuscated. Running it just prints on console the string `i -> 1`, which is not useful for now. We can either solve the obfuscation (good luck with that) or dynamically analyze it. Luckily, procmon from the Windows utils helps us out. Simply set a filter on "Process Name" to include only powershell-related entries and you will soon see that a file named "evil.exe" is being created and run.

However, there's nothing in the Temp folder, so probably "evil.exe" is set to delete itself after it runs.

### Third part of the flag: the Executable

We know that the sample is inside the Temp folder. We can set the permissions of this user of this folder to deny file deletion. Then we run evil.ps1 (which will return an error). Finally, we regain Read permissions. We will finally see evil.exe in the Temp (We'll do copies).

### Third part of the flag: The Analysis

Opening the EXE with IDA tells us that the main doesn't in fact perform any action aside from deleting the file and telling that "i -> 1". This is far from useless, however. If we look at user-defined functions, there's a weird pr1ntf (with a 1 instead of the i). If we go inside it and disassemble it, we see that a bytestring is being elaborated. Unluckily, for each operation the program sleeps 5 seconds, making the method untolerably long to execute. We'll have to get this code in a new executable, removing all the "sleep" calls. If we strip the code of the sleeps and run the pr1ntf functions, we finally get the last part of the flag: `_1s_L0nG`.

## Flag

`srdnlen{1nfect1on_ch4in_1s_L0nG}`
