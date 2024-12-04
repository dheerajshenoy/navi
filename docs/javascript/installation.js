function copyCode(button) {
    const codeContainer = button.closest('.code-container');
    const codeContent = codeContainer.querySelector('code').innerText;

    // Use the Clipboard API to copy text
    navigator.clipboard.writeText(codeContent).then(() => {
    }).catch(err => {
        console.error('Failed to copy: ', err);
    });

}