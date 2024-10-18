#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

typedef struct Contato {
    char nome[50];
    char telefone[20];
    char email[50];
} Contato;

typedef struct No {
    Contato contato;
    struct No* esquerda;
    struct No* direita;
} No;

typedef struct Widgets {
    GtkWidget* entry_nome;
    GtkWidget* entry_telefone;
    GtkWidget* entry_email;
    GtkWidget* entry_busca;
    GtkWidget* label_resultado;
    GtkWidget* box_contatos;
} Widgets;

No* raiz = NULL;


No* criarNo(char nome[], char telefone[], char email[]) {
    No* novoNo = (No*)malloc(sizeof(No));
    strcpy(novoNo->contato.nome, nome);
    strcpy(novoNo->contato.telefone, telefone);
    strcpy(novoNo->contato.email, email);
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;
    return novoNo;
}


No* inserirBST(No* raiz, char nome[], char telefone[], char email[]) {
    if (raiz == NULL) {
        return criarNo(nome, telefone, email);
    }
    
    if (strcmp(nome, raiz->contato.nome) < 0) {
        raiz->esquerda = inserirBST(raiz->esquerda, nome, telefone, email);
    } else if (strcmp(nome, raiz->contato.nome) > 0) {
        raiz->direita = inserirBST(raiz->direita, nome, telefone, email);
    }
    
    return raiz;
}


No* encontrarMenor(No* raiz) {
    if (raiz->esquerda == NULL) {
        return raiz;
    }
    return encontrarMenor(raiz->esquerda);
}
No* removerBST(No* raiz, const char* nome) {
    if (raiz == NULL) {
        return NULL; 
    }

    if (strcmp(nome, raiz->contato.nome) < 0) {
        raiz->esquerda = removerBST(raiz->esquerda, nome);
    } else if (strcmp(nome, raiz->contato.nome) > 0) {
        raiz->direita = removerBST(raiz->direita, nome);
    } else {

        if (raiz->esquerda == NULL) {
            No* temp = raiz->direita;
            free(raiz);
            return temp;
        } else if (raiz->direita == NULL) {
            No* temp = raiz->esquerda;
            free(raiz);
            return temp;
        }

        No* temp = encontrarMenor(raiz->direita);
        raiz->contato = temp->contato;
        raiz->direita = removerBST(raiz->direita, temp->contato.nome);
    }
    return raiz;
}

void exibirContatosEmOrdem(No* raiz, GtkWidget* box_contatos) {
    if (raiz != NULL) {
        exibirContatosEmOrdem(raiz->esquerda, box_contatos);

        char buffer[256];
        sprintf(buffer, "Nome: %s\nTelefone: %s\nEmail: %s", raiz->contato.nome, raiz->contato.telefone, raiz->contato.email);
        GtkWidget* label = gtk_label_new(buffer);
        gtk_box_pack_start(GTK_BOX(box_contatos), label, FALSE, FALSE, 10);

        exibirContatosEmOrdem(raiz->direita, box_contatos);
    }
}

void atualizarLista(GtkWidget* box_contatos) {
    GList* children = gtk_container_get_children(GTK_CONTAINER(box_contatos));
    for (GList* iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    if (raiz == NULL) {
        GtkWidget* label = gtk_label_new("Nenhum contato adicionado.");
        gtk_box_pack_start(GTK_BOX(box_contatos), label, FALSE, FALSE, 10);
    } else {
        exibirContatosEmOrdem(raiz, box_contatos);
    }

    gtk_widget_show_all(box_contatos);
}

void adicionarContato(GtkWidget* widget, gpointer data) {
    Widgets* widgets = (Widgets*)data;
    const char* nome = gtk_entry_get_text(GTK_ENTRY(widgets->entry_nome));
    const char* telefone = gtk_entry_get_text(GTK_ENTRY(widgets->entry_telefone));
    const char* email = gtk_entry_get_text(GTK_ENTRY(widgets->entry_email));

    raiz = inserirBST(raiz, (char*)nome, (char*)telefone, (char*)email);

    gtk_entry_set_text(GTK_ENTRY(widgets->entry_nome), "");
    gtk_entry_set_text(GTK_ENTRY(widgets->entry_telefone), "");
    gtk_entry_set_text(GTK_ENTRY(widgets->entry_email), "");

    atualizarLista(widgets->box_contatos);
}
No* buscarContatoBST(No* raiz, const char* nome) {
    if (raiz == NULL || strcmp(nome, raiz->contato.nome) == 0) {
        return raiz;
    }

    if (strcmp(nome, raiz->contato.nome) < 0) {
        return buscarContatoBST(raiz->esquerda, nome);
    } else {
        return buscarContatoBST(raiz->direita, nome);
    }
}

void buscarContato(GtkWidget* widget, gpointer data) {
    Widgets* widgets = (Widgets*)data;
    const char* nome = gtk_entry_get_text(GTK_ENTRY(widgets->entry_busca));
    No* contatoEncontrado = buscarContatoBST(raiz, nome);
    char buffer[256] = "Contato não encontrado.\n";

    if (contatoEncontrado != NULL) {
        sprintf(buffer, "Nome: %s\nTelefone: %s\nEmail: %s\n", contatoEncontrado->contato.nome, contatoEncontrado->contato.telefone, contatoEncontrado->contato.email);
    }

    gtk_label_set_text(GTK_LABEL(widgets->label_resultado), buffer);
}

void removerContato(GtkWidget* widget, gpointer data) {
    Widgets* widgets = (Widgets*)data;
    const char* nome = gtk_entry_get_text(GTK_ENTRY(widgets->entry_busca));

    raiz = removerBST(raiz, nome);

    atualizarLista(widgets->box_contatos);
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Agendamento de Contatos");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    Widgets* widgets = malloc(sizeof(Widgets));

    widgets->entry_nome = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->entry_nome), "Nome");
    gtk_box_pack_start(GTK_BOX(vbox), widgets->entry_nome, FALSE, FALSE, 0);

    widgets->entry_telefone = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->entry_telefone), "Telefone");
    gtk_box_pack_start(GTK_BOX(vbox), widgets->entry_telefone, FALSE, FALSE, 0);

    widgets->entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->entry_email), "Email");
    gtk_box_pack_start(GTK_BOX(vbox), widgets->entry_email, FALSE, FALSE, 0);

    GtkWidget* button_add = gtk_button_new_with_label("Adicionar Contato");
    gtk_box_pack_start(GTK_BOX(vbox), button_add, FALSE, FALSE, 0);

    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    widgets->box_contatos = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(scrolled_window), widgets->box_contatos);

    GtkWidget* hbox_busca = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_busca, FALSE, FALSE, 0);
    
    widgets->entry_busca = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->entry_busca), "Nome");
    gtk_box_pack_start(GTK_BOX(hbox_busca), widgets->entry_busca, TRUE, TRUE, 0);
    
    GtkWidget* button_busca = gtk_button_new();
    GtkWidget* image_busca = gtk_image_new_from_icon_name("edit-find", GTK_ICON_SIZE_BUTTON);
    gtk_widget_set_name(button_busca, "button");
    gtk_button_set_image(GTK_BUTTON(button_busca), image_busca);
    gtk_box_pack_start(GTK_BOX(hbox_busca), button_busca, FALSE, FALSE, 0);

    GtkWidget* button_remove = gtk_button_new();
    GtkWidget* image_remover = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(button_remove), image_remover);
    gtk_box_pack_start(GTK_BOX(hbox_busca), button_remove, FALSE, FALSE, 0);

    widgets->label_resultado = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), widgets->label_resultado, TRUE, TRUE, 0);

    g_signal_connect(button_add, "clicked", G_CALLBACK(adicionarContato), widgets);
    g_signal_connect(button_busca, "clicked", G_CALLBACK(buscarContato), widgets);
    g_signal_connect(button_remove, "clicked", G_CALLBACK(removerContato), widgets);


    raiz = inserirBST(raiz, "Weslem", "88 98877-7777", "weslem@gmail.com");
    raiz = inserirBST(raiz, "Jordana", "19 99912-3456", "jordana@hotmail.com");
    raiz = inserirBST(raiz, "Iasmin", "85 98855-5555", "iasmin@outlook.com");

    atualizarLista(widgets->box_contatos);

    gtk_widget_show_all(window);
    gtk_main();

    free(widgets);
    return 0;
}
