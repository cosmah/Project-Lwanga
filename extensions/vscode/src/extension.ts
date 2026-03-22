import * as vscode from 'vscode';
import * as cp from 'child_process';
import * as path from 'path';

let outputChannel: vscode.OutputChannel;

export function activate(context: vscode.ExtensionContext) {
    outputChannel = vscode.window.createOutputChannel('Lwanga');
    
    // Register format command
    context.subscriptions.push(
        vscode.commands.registerCommand('lwanga.format', formatDocument)
    );
    
    // Register add header command
    context.subscriptions.push(
        vscode.commands.registerCommand('lwanga.addHeader', addLogoHeader)
    );
    
    // Register compile command
    context.subscriptions.push(
        vscode.commands.registerCommand('lwanga.compile', compileCurrentFile)
    );
    
    // Register format on save
    context.subscriptions.push(
        vscode.workspace.onWillSaveTextDocument(event => {
            const config = vscode.workspace.getConfiguration('lwanga');
            if (config.get('formatOnSave') && event.document.languageId === 'lwanga') {
                event.waitUntil(formatDocumentProviderAsync(event.document));
            }
        })
    );

    // Register document formatter
    context.subscriptions.push(
        vscode.languages.registerDocumentFormattingEditProvider('lwanga', {
            provideDocumentFormattingEdits(document: vscode.TextDocument): Thenable<vscode.TextEdit[]> {
                return formatDocumentProviderAsync(document);
            }
        })
    );
    
    outputChannel.appendLine('Lwanga extension activated');
}

async function formatDocument() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'lwanga') {
        vscode.window.showErrorMessage('No Lwanga file is currently active');
        return;
    }
    
    const edits = await formatDocumentProviderAsync(editor.document);
    const workspaceEdit = new vscode.WorkspaceEdit();
    workspaceEdit.set(editor.document.uri, edits);
    await vscode.workspace.applyEdit(workspaceEdit);
    vscode.window.showInformationMessage('Document formatted');
}


async function formatDocumentProviderAsync(document: vscode.TextDocument): Promise<vscode.TextEdit[]> {
    const config = vscode.workspace.getConfiguration('lwanga');
    const formatterPath = config.get<string>('formatterPath', 'lwangafmt');
    try {
        // Save the document before formatting
        await document.save();
        cp.execFileSync(formatterPath, ['--write', document.fileName], {
            encoding: 'utf8',
            timeout: 5000
        });
        // Reload the document text after formatting
        const newText = document.getText();
        const fullRange = new vscode.Range(
            document.positionAt(0),
            document.positionAt(newText.length)
        );
        return [vscode.TextEdit.replace(fullRange, newText)];
    } catch (error: any) {
        outputChannel.appendLine(`Formatting error: ${error.message}`);
        vscode.window.showErrorMessage(`Failed to format: ${error.message}`);
        return [];
    }
}

async function addLogoHeader() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'lwanga') {
        vscode.window.showErrorMessage('No Lwanga file is currently active');
        return;
    }
    
    const config = vscode.workspace.getConfiguration('lwanga');
    const formatterPath = config.get<string>('formatterPath', 'lwangafmt');
    const logoStyle = config.get<string>('logoStyle', 'minimal');
    
    try {
        cp.execFileSync(formatterPath, [
            '--add-header',
            `--logo-style=${logoStyle}`,
            editor.document.fileName
        ], {
            encoding: 'utf8',
            timeout: 5000
        });
        
        vscode.window.showInformationMessage('Logo header added');
    } catch (error: any) {
        outputChannel.appendLine(`Add header error: ${error.message}`);
        vscode.window.showErrorMessage(`Failed to add header: ${error.message}`);
    }
}

async function compileCurrentFile() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'lwanga') {
        vscode.window.showErrorMessage('No Lwanga file is currently active');
        return;
    }
    
    // Save the file first
    await editor.document.save();
    
    const config = vscode.workspace.getConfiguration('lwanga');
    const compilerPath = config.get<string>('compilerPath', 'lwangac');
    const compilerArgs = config.get<string[]>('compilerArgs', ['-O2']);
    
    const inputFile = editor.document.fileName;
    const outputFile = inputFile.replace(/\.lwanga$/, '');
    
    outputChannel.clear();
    outputChannel.show(true);
    outputChannel.appendLine(`Compiling ${path.basename(inputFile)}...`);
    outputChannel.appendLine(`Command: ${compilerPath} ${compilerArgs.join(' ')} ${inputFile} -o ${outputFile}`);
    outputChannel.appendLine('');
    
    try {
        const result = cp.execFileSync(compilerPath, [
            ...compilerArgs,
            inputFile,
            '-o',
            outputFile
        ], {
            encoding: 'utf8',
            timeout: 30000,
            cwd: path.dirname(inputFile)
        });
        
        outputChannel.appendLine(result);
        outputChannel.appendLine('');
        outputChannel.appendLine('✓ Compilation successful');
        vscode.window.showInformationMessage(`Compiled successfully: ${path.basename(outputFile)}`);
    } catch (error: any) {
        outputChannel.appendLine(error.stdout || '');
        outputChannel.appendLine(error.stderr || '');
        outputChannel.appendLine('');
        outputChannel.appendLine('✗ Compilation failed');
        vscode.window.showErrorMessage('Compilation failed. See output for details.');
    }
}

export function deactivate() {
    if (outputChannel) {
        outputChannel.dispose();
    }
}
