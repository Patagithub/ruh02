/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymartin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/29 14:41:56 by ymartin           #+#    #+#             */
/*   Updated: 2023/07/29 16:41:06 by thpata           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LENGTH 10000

typedef struct {
    unsigned long long  number;
    char *word;
} NumberWord;
NumberWord numbers_dict[MAX_LENGTH];
long long dict_size = 0;

int load_dictionary(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
	return (0);
    }

    char line[MAX_LENGTH];
    while (fgets(line, MAX_LENGTH, file) != NULL) {
        // Recherche de l'indice du premier ':' dans la ligne
        char *colon_pos = strchr(line, ':');
        if (colon_pos == NULL) {
            // Gérer une ligne invalide si besoin
            continue;
        }
        
        // Extraire le nombre en convertissant la sous-chaîne avant le ':' en long long
        *colon_pos = '\0'; // Null-terminate à la position du ':'
        long long number = atoll(line);

        // Extraire le mot en copiant la sous-chaîne après le ':' dans un tableau de caractères
        char *word_start = colon_pos + 1;
        char word[MAX_LENGTH];
        size_t word_len = strlen(word_start);
        if (word_len > 0 && word_start[word_len - 1] == '\n') {
            word_start[word_len - 1] = '\0'; // Supprimer le '\n' en fin de ligne si présent
        }
        strncpy(word, word_start, MAX_LENGTH);

        // Ajouter l'entrée au dictionnaire
        numbers_dict[dict_size].number = number;
        numbers_dict[dict_size].word = strdup(word);
        dict_size++;
    }

    fclose(file);
    return (0);
}

char *number_to_words(unsigned long long number) {
    char *word = (char *)malloc(MAX_LENGTH * sizeof(char));
    strcpy(word, ""); // Initialiser la chaîne à vide

    // Vérifier si le nombre est nul
    if (number == 0) {
        strcpy(word, "zero");
        return word;
    }

    // Chercher le mot associé dans le dictionnaire
 long long i = 0;
    while (i < dict_size) {
        if (numbers_dict[i].number == number) {
            strcpy(word, numbers_dict[i].word);
            return word;
        }
        i++;
    }
    if ((long long)number < 0) {
        strcat(word, "Error");
        return word;
     }

    /*strcpy(word, "Number not found");strcpy(word, "Number not found");
    return word;*/
    // Si le nombre n'est pas trouvé dans le dictionnaire, on le construit en concaténant les mots
    if (number >= 1000000000) {
        unsigned long long billions = number / 1000000000;
        strcat(word, number_to_words(billions));
        strcat(word, " billion ");
        number %= 1000000000;
    }

    if (number >= 1000000) {
        unsigned long long millions = number / 1000000;
        strcat(word, number_to_words(millions));
        strcat(word, " million ");
        number %= 1000000;
    }

    if (number >= 1000) {
        unsigned long long thousands = number / 1000;
        strcat(word, number_to_words(thousands));
        strcat(word, " thousand ");
        number %= 1000;
    }

    if (number >= 100) {
        unsigned long long hundreds = number / 100;
        strcat(word, number_to_words(hundreds));
        strcat(word, " hundred ");
        number %= 100;
    }

    if (number > 0) {
        if (strlen(word) > 0) {
            strcat(word, "and ");
        }
        if (number <= 20) {
            strcat(word, number_to_words(number % 20));
        } else {
            long long tens = (number / 10) * 10;
            long long units = number % 10;
            strcat(word, number_to_words(tens));
            if (units > 0) {
                strcat(word, "-");
                strcat(word, number_to_words(units));
            }
        }
    }

    return word;
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        char usage[] = "Utilisation : ";
        size_t usage_len = strlen(usage);
        write(1, usage, usage_len); // Écrire sur la sortie standard (1) au lieu d'utiliser printf

        size_t prog_name_len = strlen(argv[0]);
        write(1, argv[0], prog_name_len); // Écrire sur la sortie standard (1) au lieu d'utiliser printf

        char newline = '\n';
        write(1, &newline, 1); // Écrire sur la sortie standard (1) au lieu d'utiliser printf
        return 1;
    }

    const char *dict_file = "numbers.dict";
    load_dictionary(dict_file);

    unsigned long long number = atoll(argv[1]);
    if (number == 0 || number < 0) {
        // Afficher le message d'erreur si l'argument est nul ou négatif
        char error_msg[] = "Erreur : Veuillez fournir un nombre entier positif non nul.\n";
        size_t error_msg_len = strlen(error_msg);
        write(1, error_msg, error_msg_len);
        return 1;
    }

    char *result = number_to_words(number);
    if (strcmp(result, "Number not found") == 0) {
        // Print the dictionary error message for unresolved number
        char dict_error_msg[] = "Dict Error\n";
        size_t dict_error_msg_len = strlen(dict_error_msg);
        write(1, dict_error_msg, dict_error_msg_len);
        free(result);
        return 1;
    }


    size_t result_len = strlen(result);
    write(1, result, result_len); // Écrire sur la sortie standard (1) au lieu d'utiliser printf

    char newline = '\n';
    write(1, &newline, 1); // Écrire sur la sortie standard (1) au lieu d'utiliser printf

    free(result); // Libérer la mémoire allouée dynamiquement pour le mot
    // Libérer la mémoire allouée pour les mots dans numbers_dict
    int i = 0;
	while (i < dict_size) {
    free(numbers_dict[i].word);
    i++;
}
    return 0;
}
