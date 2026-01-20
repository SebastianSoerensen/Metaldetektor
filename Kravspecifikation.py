import pandas as pd

df = pd.read_excel("Kravspecifikation metaldetektor.xlsx")

# Rens typiske syndere
df = df.replace("&", "og", regex=True)
df = df.replace("\t", " ", regex=True)
df = df.replace("�", "", regex=True)

latex = df.to_latex(
    index=False,
    column_format="p{1.2cm} p{5cm} p{8cm}"
)

with open("tabel.tex", "w", encoding="utf-8") as f:
    f.write(latex)
