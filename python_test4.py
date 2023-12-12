import bss_module_december_sixth

print("Check1")

python_API = bss_module_december_sixth.PythonAPI()

print("Check2")

iErr = python_API.run_bss2_simulator(
    '/Users/eric/CLionProjects/BusinessStrategy2.0/WorkingFiles/Config/default.json')

print("Check3")

print(f"Finished processing with error code {iErr}")
