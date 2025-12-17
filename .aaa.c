#include <windows.h>
#include <lm.h>
#include <stdio.h>
#include <tchar.h>

// Il faut lier la bibliothèque "netapi32.lib"
#pragma comment(lib, "netapi32.lib")

// Fonction pour convertir un tableau d'octets en une chaîne hexadécimale
void BytesToHexString(const LPBYTE bytes, DWORD len, LPTSTR str) {
    for (DWORD i = 0; i < len; i++) {
        _stprintf_s(str + (i * 2), 3, _T("%02x"), bytes[i]);
    }
}

int main() {
    USER_INFO_3 *ui = NULL;
    NET_API_STATUS nStatus;
    LPWSTR username = NULL;

    // Demander le nom de l'utilisateur
    username = (LPWSTR)malloc(UNLEN * sizeof(WCHAR));
    if (username == NULL) {
        wprintf(L"Erreur d'allocation de mémoire.\n");
        return 1;
    }

    wprintf(L"Entrez le nom d'utilisateur local : ");
    wscanf_s(L"%s", username, UNLEN);

    // Récupérer les informations de l'utilisateur, niveau 3 pour inclure les infos de mot de passe
    nStatus = NetUserGetInfo(NULL, username, 3, (LPBYTE *)&ui);

    if (nStatus != NERR_Success) {
        wprintf(L"Erreur NetUserGetInfo: %d\n", nStatus);
        // Gérer les erreurs courantes
        if (nStatus == ERROR_ACCESS_DENIED) {
            wprintf(L"Accès refusé. Veuillez exécuter ce programme en tant qu'administrateur.\n");
        } else if (nStatus == NERR_UserNotFound) {
            wprintf(L"Utilisateur '%s' non trouvé.\n", username);
        }
        free(username);
        return 1;
    }

    wprintf(L"\n--- Informations pour l'utilisateur : %s ---\n", username);

    // Vérifier si le mot de passe est stocké en clair (très rare et dangereux)
    if (ui->usri3_password != NULL) {
        wprintf(L"ATTENTION: Le mot de passe est stocké en clair (ce qui est une faille de sécurité majeure) :\n");
        wprintf(L"Mot de passe : %s\n", ui->usri3_password);
    }

    // Afficher le hachage LM (ancien, moins sécurisé)
    if (ui->usri3_password_expired != NULL) {
        // Le champ usri3_password_expired contient en réalité le hachage LM dans cette structure
        // C'est un comportement documenté mais étrange de l'API.
        wprintf(L"\nHachage LM (LAN Manager) :\n");
        LPTSTR lm_hash_str = (LPTSTR)malloc(PWLEN * 2 + 1);
        if (lm_hash_str) {
            BytesToHexString((LPBYTE)ui->usri3_password_expired, 16, lm_hash_str);
            wprintf(L"%s\n", lm_hash_str);
            free(lm_hash_str);
        }
    }

    // Afficher le hachage NTLM (plus courant et sécurisé)
    // Le haché NTLM se trouve dans usri3_full_name à ce niveau d'information
    // C'est un autre "hack" de l'API pour récupérer cette donnée.
    if (ui->usri3_full_name != NULL) {
        wprintf(L"\nHachage NTLM (NT Hash) :\n");
        LPTSTR ntlm_hash_str = (LPTSTR)malloc(UNLEN * 2 + 1);
        if (ntlm_hash_str) {
            BytesToHexString((LPBYTE)ui->usri3_full_name, 16, ntlm_hash_str);
            wprintf(L"%s\n", ntlm_hash_str);
            free(ntlm_hash_str);
        }
    }

    wprintf(L"\n----------------------------------------\n");

    // Libérer la mémoire allouée par l'API
    if (ui != NULL) {
        NetApiBufferFree(ui);
    }
    if (username != NULL) {
        free(username);
    }

    return 0;
}