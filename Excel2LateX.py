import pandas as pd

Tabel = ["TABEL1", "TABEL2", "TABEL3", "TABEL4", "TABEL5", "TABEL6"]

def excel_to_latex(tabel):
    # Byg filnavn til Excel
    excel_fil = f"{tabel}.xlsx"
    df = pd.read_excel(excel_fil)

    # Rens typiske syndere
    df = df.replace("&", "og", regex=True)
    df = df.replace("\t", " ", regex=True)
    df = df.replace("�", "", regex=True)
    df = df.replace("<", "$<$", regex=True)

    latex = df.to_latex(
        index=False,
        column_format="p{1.2cm} p{5cm} p{8cm}"
    )

    # Byg filnavn til output
    tex_fil = f"{tabel}.tex"
    with open(tex_fil, "w", encoding="utf-8") as f:
        f.write(latex)

    print(f"Genereret: {tex_fil}")


for t in Tabel:
    excel_to_latex(t)
