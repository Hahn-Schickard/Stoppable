import subprocess
from typing import List


def is_installed(executable: str, throw_on_failure=True):
    try:
        subprocess.run([executable, '--version'],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError:
        error_msg = 'Program ' + executable + ' is not installed.'
        if throw_on_failure:
            raise FileNotFoundError(error_msg)
        else:
            return error_msg


class PIPE_Value:
    def __init__(self, stdout: str, stderr: str):
        self.stdout = stdout
        self.stderr = stderr


def run_process(executable: str, arguments: List[str] = [], encoding='utf-8', throw_on_failure=True):
    is_installed(executable)
    command = [executable]
    if arguments:
        command.extend(arguments)
    try:
        process = subprocess.run(command, stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE, encoding=encoding, universal_newlines=True)
        if throw_on_failure:
            if process.stderr:
                error_msg = 'Running command ' + \
                    ' '.join(command) + ' returned an error: ' + \
                    process.stderr
                raise OSError(process.returncode, error_msg)
            else:
                return PIPE_Value(process.stdout, str())
        else:
            return PIPE_Value(process.stdout, process.stderr)
    except subprocess.CalledProcessError as exception:
        raise RuntimeError('An exception occurred while running command: ' +
                           ' '.join(command) + ' Exception is: ' + exception.output)
